#include "pathfinding.h"
#include <stdio.h>

int main(void)
{
    graph_t *graph;
    vertex_t *A, *B, *C;
    queue_t *path;
    
    printf("Creating graph...\n");
    graph = graph_create();
    
    /* Add vertices */
    A = graph_add_vertex(graph, "A", 0, 0);
    B = graph_add_vertex(graph, "B", 1, 0);
    C = graph_add_vertex(graph, "C", 2, 0);
    
    /* Add edges - check if 4th or 5th parameter is needed */
    /* Try with 5 parameters if that's what the header says */
    graph_add_edge(graph, "A", "B", 1, 0);  /* 0 = undirected? */
    graph_add_edge(graph, "B", "C", 2, 0);
    graph_add_edge(graph, "A", "C", 5, 0);
    
    printf("Running Dijkstra...\n");
    path = dijkstra_graph(graph, A, C);
    
    if (path)
    {
        printf("Path found with %lu nodes\n", path->size);
        queue_delete(path);
    }
    else
    {
        printf("No path found\n");
    }
    
    graph_delete(graph);
    return (0);
}