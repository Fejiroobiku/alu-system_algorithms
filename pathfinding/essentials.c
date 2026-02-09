#include "queues.h"
#include "graphs.h"
#include <stdlib.h>
#include <string.h>

/* Queue functions */
queue_t *queue_create(void)
{
    queue_t *queue = malloc(sizeof(queue_t));
    if (!queue)
        return NULL;
    
    queue->front = NULL;
    queue->back = NULL;
    return queue;
}

queue_node_t *queue_push_front(queue_t *queue, void *ptr)
{
    queue_node_t *new_node;
    
    if (!queue)
        return NULL;
    
    new_node = malloc(sizeof(queue_node_t));
    if (!new_node)
        return NULL;
    
    new_node->ptr = ptr;
    new_node->prev = NULL;
    new_node->next = queue->front;
    
    if (queue->front)
        queue->front->prev = new_node;
    else
        queue->back = new_node;
    
    queue->front = new_node;
    return new_node;
}

queue_node_t *queue_push_back(queue_t *queue, void *ptr)
{
    queue_node_t *new_node;
    
    if (!queue)
        return NULL;
    
    new_node = malloc(sizeof(queue_node_t));
    if (!new_node)
        return NULL;
    
    new_node->ptr = ptr;
    new_node->next = NULL;
    new_node->prev = queue->back;
    
    if (queue->back)
        queue->back->next = new_node;
    else
        queue->front = new_node;
    
    queue->back = new_node;
    return new_node;
}

void *dequeue(queue_t *queue)
{
    queue_node_t *front_node;
    void *ptr;
    
    if (!queue || !queue->front)
        return NULL;
    
    front_node = queue->front;
    ptr = front_node->ptr;
    queue->front = front_node->next;
    
    if (queue->front)
        queue->front->prev = NULL;
    else
        queue->back = NULL;
    
    free(front_node);
    return ptr;
}

void queue_delete(queue_t *queue)
{
    queue_node_t *current, *next;
    
    if (!queue)
        return;
    
    current = queue->front;
    while (current)
    {
        next = current->next;
        free(current);
        current = next;
    }
    free(queue);
}

/* Graph functions - without type member */
graph_t *graph_create(void)
{
    graph_t *graph = malloc(sizeof(graph_t));
    if (!graph)
        return NULL;
    
    graph->nb_vertices = 0;
    graph->vertices = NULL;
    return graph;
}

vertex_t *graph_add_vertex(graph_t *graph, char const *str, int x, int y)
{
    vertex_t *new_vertex;
    
    if (!graph || !str)
        return NULL;
    
    new_vertex = malloc(sizeof(vertex_t));
    if (!new_vertex)
        return NULL;
    
    new_vertex->content = strdup(str);
    if (!new_vertex->content)
    {
        free(new_vertex);
        return NULL;
    }
    
    new_vertex->x = x;
    new_vertex->y = y;
    new_vertex->nb_edges = 0;
    new_vertex->edges = NULL;
    new_vertex->next = graph->vertices;
    graph->vertices = new_vertex;
    graph->nb_vertices++;
    
    return new_vertex;
}

int graph_add_edge(graph_t *graph, char const *src,
                   char const *dest, int weight, edge_type_t type)
{
    vertex_t *src_vertex = NULL, *dest_vertex = NULL;
    vertex_t *vertex;
    edge_t *new_edge;
    
    (void)type; /* Type parameter is ignored since edge_t has no type member */
    
    if (!graph || !src || !dest)
        return 0;
    
    /* Find source and destination vertices */
    vertex = graph->vertices;
    while (vertex)
    {
        if (strcmp(vertex->content, src) == 0)
            src_vertex = vertex;
        if (strcmp(vertex->content, dest) == 0)
            dest_vertex = vertex;
        if (src_vertex && dest_vertex)
            break;
        vertex = vertex->next;
    }
    
    if (!src_vertex || !dest_vertex)
        return 0;
    
    /* Create edge from src to dest */
    new_edge = malloc(sizeof(edge_t));
    if (!new_edge)
        return 0;
    
    new_edge->dest = dest_vertex;
    new_edge->weight = weight;
    new_edge->next = src_vertex->edges;
    src_vertex->edges = new_edge;
    src_vertex->nb_edges++;
    
    return 1;
}

void graph_delete(graph_t *graph)
{
    vertex_t *vertex, *next_vertex;
    edge_t *edge, *next_edge;
    
    if (!graph)
        return;
    
    vertex = graph->vertices;
    while (vertex)
    {
        next_vertex = vertex->next;
        
        edge = vertex->edges;
        while (edge)
        {
            next_edge = edge->next;
            free(edge);
            edge = next_edge;
        }
        
        free(vertex->content);
        free(vertex);
        vertex = next_vertex;
    }
    
    free(graph);
}