#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Define all structures we need */
typedef struct queue_node_s
{
    void *ptr;
    struct queue_node_s *next;
    struct queue_node_s *prev;
} queue_node_t;

typedef struct queue_s
{
    queue_node_t *front;
    queue_node_t *back;
} queue_t;

typedef struct edge_s
{
    struct vertex_s *dest;
    struct edge_s *next;
    int weight;
} edge_t;

typedef struct vertex_s
{
    size_t index;
    char *content;
    int x;
    int y;
    size_t nb_edges;
    edge_t *edges;
    struct vertex_s *next;
} vertex_t;

typedef struct graph_s
{
    size_t nb_vertices;
    vertex_t *vertices;
} graph_t;

typedef enum edge_type_e
{
    UNIDIRECTIONAL = 0,
    BIDIRECTIONAL
} edge_type_t;

/* Queue functions */
queue_t *queue_create(void)
{
    queue_t *queue = malloc(sizeof(queue_t));
    if (queue)
    {
        queue->front = NULL;
        queue->back = NULL;
    }
    return queue;
}

int enqueue(queue_t *queue, void *ptr)
{
    queue_node_t *node;
    
    if (!queue)
        return (0);
    
    node = malloc(sizeof(queue_node_t));
    if (!node)
        return (0);
    
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

void *dequeue(queue_t *queue)
{
    queue_node_t *node;
    void *ptr;
    
    if (!queue || !queue->front)
        return (NULL);
    
    node = queue->front;
    ptr = node->ptr;
    
    queue->front = node->next;
    if (queue->front)
        queue->front->prev = NULL;
    else
        queue->back = NULL;
    
    free(node);
    return (ptr);
}

void queue_delete(queue_t *queue)
{
    if (!queue)
        return;
    
    while (queue->front)
        free(dequeue(queue));
    
    free(queue);
}

/* Graph functions */
graph_t *graph_create(void)
{
    graph_t *graph = malloc(sizeof(graph_t));
    if (graph)
    {
        graph->nb_vertices = 0;
        graph->vertices = NULL;
    }
    return graph;
}

vertex_t *graph_add_vertex(graph_t *graph, char const *str, int x, int y)
{
    vertex_t *vertex, *temp;
    
    if (!graph || !str)
        return (NULL);
    
    /* Check if vertex already exists */
    temp = graph->vertices;
    while (temp)
    {
        if (strcmp(temp->content, str) == 0)
            return temp;
        temp = temp->next;
    }
    
    /* Create new vertex */
    vertex = malloc(sizeof(vertex_t));
    if (!vertex)
        return (NULL);
    
    vertex->content = strdup(str);
    if (!vertex->content)
    {
        free(vertex);
        return (NULL);
    }
    
    vertex->x = x;
    vertex->y = y;
    vertex->index = graph->nb_vertices;
    vertex->nb_edges = 0;
    vertex->edges = NULL;
    vertex->next = graph->vertices;
    graph->vertices = vertex;
    graph->nb_vertices++;
    
    return vertex;
}

int graph_add_edge(graph_t *graph, char const *src, char const *dest,
                   int weight, edge_type_t type)
{
    vertex_t *src_vertex = NULL, *dest_vertex = NULL;
    vertex_t *temp;
    edge_t *edge;
    
    if (!graph || !src || !dest)
        return (0);
    
    /* Find source and destination vertices */
    temp = graph->vertices;
    while (temp && (!src_vertex || !dest_vertex))
    {
        if (strcmp(temp->content, src) == 0)
            src_vertex = temp;
        if (strcmp(temp->content, dest) == 0)
            dest_vertex = temp;
        temp = temp->next;
    }
    
    if (!src_vertex || !dest_vertex)
        return (0);
    
    /* Add edge from src to dest */
    edge = malloc(sizeof(edge_t));
    if (!edge)
        return (0);
    
    edge->dest = dest_vertex;
    edge->weight = weight;
    edge->next = src_vertex->edges;
    src_vertex->edges = edge;
    src_vertex->nb_edges++;
    
    /* If bidirectional, add edge from dest to src */
    if (type == BIDIRECTIONAL)
    {
        edge = malloc(sizeof(edge_t));
        if (!edge)
            return (1); /* At least one edge was added */
        
        edge->dest = src_vertex;
        edge->weight = weight;
        edge->next = dest_vertex->edges;
        dest_vertex->edges = edge;
        dest_vertex->nb_edges++;
    }
    
    return (1);
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
        
        /* Free all edges */
        edge = vertex->edges;
        while (edge)
        {
            next_edge = edge->next;
            free(edge);
            edge = next_edge;
        }
        
        /* Free vertex content and vertex itself */
        free(vertex->content);
        free(vertex);
        
        vertex = next_vertex;
    }
    
    free(graph);
}

/* Backtracking algorithm */
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
    
    if (!enqueue(path, content_copy))
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
            
            /* Backtrack: remove vertices added during unsuccessful exploration */
            while (path->back && 
                   strcmp((char *)path->back->ptr, current->content) != 0)
            {
                queue_node_t *node = path->back;
                path->back = node->prev;
                if (path->back)
                    path->back->next = NULL;
                else
                    path->front = NULL;
                
                free(node->ptr);
                free(node);
            }
        }
        edge = edge->next;
    }
    
    /* If we get here, this path didn't work, remove current vertex */
    if (path->back && strcmp((char *)path->back->ptr, current->content) == 0)
    {
        queue_node_t *node = path->back;
        path->back = node->prev;
        if (path->back)
            path->back->next = NULL;
        else
            path->front = NULL;
        
        free(node->ptr);
        free(node);
    }
    
    return (0);
}

queue_t *backtracking_graph(graph_t *graph, vertex_t const *start,
                            vertex_t const *target)
{
    queue_t *path;
    
    if (!graph || !start || !target)
        return (NULL);
    
    path = queue_create();
    if (!path)
        return (NULL);
    
    /* Start recursive backtracking */
    if (!backtrack_recursive(start, target, path))
    {
        /* Clean up if no path found */
        while (path->front)
        {
            char *content = (char *)dequeue(path);
            free(content);
        }
        queue_delete(path);
        return (NULL);
    }
    
    return (path);
}

/* Test main function */
static void print_free_path(queue_t *path)
{
    printf("Path found:\n");
    while (path->front)
    {
        char *city = (char *)dequeue(path);
        printf(" %s\n", city);
        free(city);
    }
    free(path);
}

int main(void)
{
    graph_t *graph;
    vertex_t *start, *target;
    queue_t *path;

    graph = graph_create();
    graph_add_vertex(graph, "San Francisco", 0, 0);
    graph_add_vertex(graph, "Los Angeles", 0, 0);
    graph_add_vertex(graph, "Las Vegas", 0, 0);
    start = graph_add_vertex(graph, "Seattle", 0, 0);
    graph_add_vertex(graph, "Chicago", 0, 0);
    graph_add_vertex(graph, "Washington", 0, 0);
    graph_add_vertex(graph, "New York", 0, 0);
    graph_add_vertex(graph, "Houston", 0, 0);
    graph_add_vertex(graph, "Nashville", 0, 0);
    target = graph_add_vertex(graph, "Miami", 0, 0);
    graph_add_edge(graph, "San Francisco", "Los Angeles", 0, BIDIRECTIONAL);
    graph_add_edge(graph, "San Francisco", "Las Vegas", 0, BIDIRECTIONAL);
    graph_add_edge(graph, "Los Angeles", "Las Vegas", 0, BIDIRECTIONAL);
    graph_add_edge(graph, "San Francisco", "Seattle", 0, BIDIRECTIONAL);
    graph_add_edge(graph, "Seattle", "Chicago", 0, BIDIRECTIONAL);
    graph_add_edge(graph, "Chicago", "Washington", 0, BIDIRECTIONAL);
    graph_add_edge(graph, "Washington", "New York", 0, BIDIRECTIONAL);
    graph_add_edge(graph, "Las Vegas", "Houston", 0, BIDIRECTIONAL);
    graph_add_edge(graph, "Houston", "Nashville", 0, BIDIRECTIONAL);
    graph_add_edge(graph, "Nashville", "Washington", 0, BIDIRECTIONAL);
    graph_add_edge(graph, "Nashville", "Miami", 0, BIDIRECTIONAL);

    path = backtracking_graph(graph, start, target);
    if (!path)
    {
        fprintf(stderr, "Failed to retrieve path\n");
        return (1);
    }

    print_free_path(path);
    graph_delete(graph);

    return (0);
}