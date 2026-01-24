# build-all.ps1 - Build all graph tasks
Write-Host "=== Graphs Project Builder ===" -ForegroundColor Cyan

# First, ensure graph_display.c exists
if (-not (Test-Path "graph_display.c")) {
    Write-Host "Creating graph_display.c..." -ForegroundColor Yellow
    @'#include "graphs.h"
#include <stdio.h>

void graph_display(const graph_t *graph)
{
    vertex_t *v;
    edge_t *e;
    
    if (!graph) {
        printf("(null)\n");
        return;
    }
    
    printf("Graph: %lu vertices\n", graph->nb_vertices);
    
    v = graph->vertices;
    while (v) {
        printf("  [%lu] '%s' (edges: %d) -> ", 
               v->index, 
               v->content ? v->content : "(nil)",
               v->nb_edges);
        
        e = v->edges;
        while (e) {
            printf("%lu", e->dest->index);
            e = e->next;
            if (e) printf(", ");
        }
        if (!v->edges) printf("(nil)");
        printf("\n");
        v = v->next;
    }
}
'@ | Out-File graph_display.c -Encoding ASCII
}

# Build rules from Makefile
$tasks = @{
    "0" = @("0-main.c", "0-graph_create.c")
    "1" = @("1-main.c", "1-graph_add_vertex.c", "0-graph_create.c", "graph_display.c", "3-graph_delete.c")
    "2" = @("2-main.c", "2-graph_add_edge.c", "1-graph_add_vertex.c", "0-graph_create.c", "graph_display.c", "3-graph_delete.c")
    "3" = @("3-main.c", "3-graph_delete.c", "0-graph_create.c", "1-graph_add_vertex.c", "2-graph_add_edge.c", "graph_display.c")
    "4" = @("4-main.c", "4-depth_first_traverse.c", "3-graph_delete.c", "0-graph_create.c", "1-graph_add_vertex.c", "2-graph_add_edge.c", "graph_display.c")
    "5" = @("5-main.c", "5-breadth_first_traverse.c", "3-graph_delete.c", "0-graph_create.c", "1-graph_add_vertex.c", "2-graph_add_edge.c", "graph_display.c")
}

Write-Host "`nBuilding tasks..." -ForegroundColor Yellow

foreach ($task in $tasks.Keys | Sort-Object) {
    Write-Host "Task $task : " -NoNewline -ForegroundColor Cyan
    
    # Check if all files exist
    $missing = @()
    foreach ($file in $tasks[$task]) {
        if (-not (Test-Path $file)) {
            $missing += $file
        }
    }
    
    if ($missing.Count -gt 0) {
        Write-Host "MISSING FILES" -ForegroundColor Red
        continue
    }
    
    # Compile
    $cmd = "gcc -Wall -Werror -Wextra -pedantic -ggdb3 -o $task.exe " + ($tasks[$task] -join " ")
    Invoke-Expression $cmd 2>&1 | Out-Null
    
    if ($LASTEXITCODE -eq 0 -and (Test-Path "$task.exe")) {
        Write-Host "BUILT ✓" -ForegroundColor Green
    } else {
        Write-Host "FAILED ✗" -ForegroundColor Red
    }
}

Write-Host "`n=== Summary ===" -ForegroundColor Cyan
Write-Host "Executables created:" -ForegroundColor Yellow
Get-ChildItem *.exe | ForEach-Object { 
    $task = $_.Name -replace '\.exe', ''
    Write-Host "  Task $task : $($_.Name)" -ForegroundColor Green 
}

Write-Host "`nRun any task with: .\[task#].exe" -ForegroundColor Green
Write-Host "Example: .\1.exe  (for Task 1)" -ForegroundColor Gray