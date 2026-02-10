#include "pathfinding.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * is_vertex_in_path - Checks if a vertex is already in the current path
 * @path: Queue containing the current path
 * @vertex: Vertex to check
 * 
 * Return: 1 if vertex is in path, 0 otherwise
 */
static int is_vertex_in_path(queue_t *path, const vertex_t *vertex)
{
    queue_node_t *node;
    
    if (!path || !path->front || !vertex)
        return (0);
    
    node = path->front;
    while (node)
    {
        if (node->ptr && strcmp((char *)node->ptr, vertex->content) == 0)
            return (1);
        node = node->next;
    }
    
    return (0);
}

/**
 * add_to_queue - Adds an element to the queue
 * @queue: Queue to add to
 * @ptr: Pointer to add
 * 
 * Return: 1 on success, 0 on failure
 */
static int add_to_queue(queue_t *queue, void *ptr)
{
    queue_node_t *node = malloc(sizeof(queue_node_t));
    
    if (!queue || !node)
    {
        free(node);
        return (0);
    }
    
    node->ptr = ptr;
    node->next = NULL;
    node->prev = queue->back;
    
    if (queue->back)
        queue->back->next = node;
    else
        queue->front = node;
    
    queue->back = node;
    return (1);
}

/**
 * remove_from_back - Removes the last element from the queue
 * @queue: Queue to remove from
 * 
 * Return: Nothing
 */
static void remove_from_back(queue_t *queue)
{
    queue_node_t *node;
    
    if (!queue || !queue->back)
        return;
    
    node = queue->back;
    queue->back = node->prev;
    
    if (queue->back)
        queue->back->next = NULL;
    else
        queue->front = NULL;
    
    free(node->ptr);
    free(node);
}

/**
 * backtrack_recursive - Recursive helper function for backtracking
 * @current: Current vertex
 * @target: Target vertex
 * @path: Current path (queue)
 * 
 * Return: 1 if path to target found from current vertex, 0 otherwise
 */
static int backtrack_recursive(const vertex_t *current, const vertex_t *target,
                               queue_t *path)
{
    edge_t *edge;
    char *content_copy;
    
    printf("Checking %s\n", current->content);
    
    /* Add current vertex to path */
    content_copy = strdup(current->content);
    if (!content_copy)
        return (0);
    
    if (!add_to_queue(path, content_copy))
    {
        free(content_copy);
        return (0);
    }
    
    /* Check if we reached the target */
    if (strcmp(current->content, target->content) == 0)
        return (1);
    
    /* Explore all edges from current vertex */
    edge = current->edges;
    while (edge)
    {
        /* Only explore if vertex not already in current path */
        if (!is_vertex_in_path(path, edge->dest))
        {
            if (backtrack_recursive(edge->dest, target, path))
                return (1);
            
            /* Backtrack: remove vertices until we're back to current */
            while (path->back && 
                   strcmp((char *)path->back->ptr, current->content) != 0)
            {
                remove_from_back(path);
            }
        }
        edge = edge->next;
    }
    
    /* If we get here, this path didn't work, remove current vertex */
    if (path->back && strcmp((char *)path->back->ptr, current->content) == 0)
    {
        remove_from_back(path);
    }
    
    return (0);
}

/**
 * backtracking_graph - Finds the first path from start to target using backtracking
 * @graph: Pointer to the graph
 * @start: Pointer to the starting vertex
 * @target: Pointer to the target vertex
 * 
 * Return: Queue containing the path from start to target, or NULL if no path found
 */
queue_t *backtracking_graph(graph_t *graph, vertex_t const *start,
                            vertex_t const *target)
{
    queue_t *path;
    
    if (!graph || !start || !target)
        return (NULL);
    
    /* Create queue */
    path = malloc(sizeof(queue_t));
    if (!path)
        return (NULL);
    
    path->front = NULL;
    path->back = NULL;
    
    /* Start recursive backtracking */
    if (!backtrack_recursive(start, target, path))
    {
        /* Clean up if no path found */
        while (path->front)
        {
            queue_node_t *node = path->front;
            path->front = node->next;
            free(node->ptr);
            free(node);
        }
        free(path);
        return (NULL);
    }
    
    return (path);
}