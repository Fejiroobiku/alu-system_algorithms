#include "pathfinding.h"
#include "queues.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * is_visited - Checks if a vertex has been visited
 * @visited: Array tracking visited vertices
 * @vertex: Vertex to check
 * Return: 1 if visited, 0 otherwise
 */
static int is_visited(char *visited, vertex_t const *vertex)
{
    size_t idx = (size_t)vertex->content % 1000;
    return visited[idx];
}

/**
 * mark_visited - Marks a vertex as visited
 * @visited: Array tracking visited vertices
 * @vertex: Vertex to mark
 */
static void mark_visited(char *visited, vertex_t const *vertex)
{
    size_t idx = (size_t)vertex->content % 1000;
    visited[idx] = 1;
}

/**
 * backtrack_recursive - Recursive helper for backtracking
 * @graph: Graph structure
 * @current: Current vertex
 * @target: Target vertex
 * @path: Path queue being built
 * @visited: Visited vertices array
 * Return: 1 if path found, 0 otherwise
 */
static int backtrack_recursive(graph_t *graph,
                               vertex_t const *current,
                               vertex_t const *target,
                               queue_t *path,
                               char *visited)
{
    edge_t const *edge;
    vertex_t const *neighbor;
    
    (void)graph;
    
    mark_visited(visited, current);
    
    if (!queue_push_back(path, (void *)current->content))
        return 0;
    
    if (current == target)
    {
        printf("Found %s\n", (char *)current->content);
        return 1;
    }
    
    printf("Checking %s\n", (char *)current->content);
    
    edge = current->edges;
    while (edge)
    {
        neighbor = edge->dest;
        
        if (is_visited(visited, neighbor))
        {
            edge = edge->next;
            continue;
        }
        
        if (backtrack_recursive(graph, neighbor, target, path, visited))
            return 1;
        
        edge = edge->next;
    }
    
    free(dequeue(path));
    return 0;
}

/**
 * backtracking_graph - Performs backtracking to find a path between vertices
 * @graph: Pointer to the graph to go through
 * @start: Pointer to the starting vertex
 * @target: Pointer to the target vertex
 *
 * Return: Queue containing the path from start to target, or NULL
 */
queue_t *backtracking_graph(graph_t *graph,
                            vertex_t const *start,
                            vertex_t const *target)
{
    queue_t *path = NULL;
    char *visited = NULL;
    
    if (!graph || !start || !target)
        return NULL;
    
    printf("Checking path from %s to %s\n",
           (char *)start->content,
           (char *)target->content);
    
    visited = calloc(1000, sizeof(char));
    if (!visited)
        return NULL;
    
    path = queue_create();
    if (!path)
    {
        free(visited);
        return NULL;
    }
    
    if (backtrack_recursive(graph, start, target, path, visited))
    {
        free(visited);
        return path;
    }
    
    free(visited);
    while (path->front)
        free(dequeue(path));
    queue_delete(path);
    return NULL;
}