# build.ps1 - Simple build script for graphs
param([string]$task = "1")

Write-Host "Building Task $task..." -ForegroundColor Yellow

# Make sure graph_display.c exists
if (-not (Test-Path "graph_display.c")) {
    @"
#include "graphs.h"
#include <stdio.h>
void graph_display(const graph_t *g) {
    if (!g) { printf("(null)\n"); return; }
    printf("Graph with %lu vertices\n", g->nb_vertices);
}
"@ | Out-File graph_display.c -Encoding ASCII
}

# Build based on task number
switch ($task) {
    "0" { $files = "0-main.c 0-graph_create.c" }
    "1" { $files = "1-main.c 1-graph_add_vertex.c 0-graph_create.c graph_display.c 3-graph_delete.c" }
    "2" { $files = "2-main.c 2-graph_add_edge.c 1-graph_add_vertex.c 0-graph_create.c graph_display.c 3-graph_delete.c" }
    "3" { $files = "3-main.c 3-graph_delete.c 0-graph_create.c 1-graph_add_vertex.c 2-graph_add_edge.c graph_display.c" }
    "4" { $files = "4-main.c 4-depth_first_traverse.c 3-graph_delete.c 0-graph_create.c 1-graph_add_vertex.c 2-graph_add_edge.c graph_display.c" }
    "5" { $files = "5-main.c 5-breadth_first_traverse.c 3-graph_delete.c 0-graph_create.c 1-graph_add_vertex.c 2-graph_add_edge.c graph_display.c" }
    default { 
        Write-Host "Error: Task must be 0-5" -ForegroundColor Red
        exit 1
    }
}

Write-Host "Compiling: $files" -ForegroundColor Gray
gcc -Wall -Werror -Wextra -pedantic -ggdb3 -o $task.exe $files.split(" ")

if ($LASTEXITCODE -eq 0) {
    Write-Host "Success! Running..." -ForegroundColor Green
    .\$task.exe
} else {
    Write-Host "Compilation failed" -ForegroundColor Red
}
