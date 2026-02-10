#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

/* Define all structures */
typedef struct queue_node_s {
    void *ptr;
    struct queue_node_s *next;
    struct queue_node_s *prev;
} queue_node_t;

typedef struct queue_s {
    queue_node_t *front;
    queue_node_t *back;
} queue_t;

typedef struct edge_s {
    struct vertex_s *dest;
    struct edge_s *next;
    int weight;
} edge_t;

typedef struct vertex_s {
    char *content;
    struct edge_s *edges;
    struct vertex_s *next;
} vertex_t;

typedef struct graph_s {
    struct vertex_s *vertices;
    size_t nb_vertices;
} graph_t;

typedef enum edge_type_e {
    UNIDIRECTIONAL = 0,
    BIDIRECTIONAL
} edge_type_t;

/* Dijkstra vertex structure */
typedef struct dijkstra_vertex_s {
    vertex_t *vertex;
    size_t cml_weight;  /* cumulative weight */
    vertex_t *path_via; /* previous vertex in path */
} dijkstra_vertex_t;

/* Queue functions */
queue_t *queue_create(void) {
    queue_t *queue = malloc(sizeof(queue_t));
    if (queue) {
        queue->front = NULL;
        queue->back = NULL;
    }
    return queue;
}

int queue_push_front(queue_t *queue, void *ptr) {
    queue_node_t *node = malloc(sizeof(queue_node_t));
    if (!queue || !node) {
        free(node);
        return 0;
    }
    
    node->ptr = ptr;
    node->next = queue->front;
    node->prev = NULL;
    
    if (queue->front)
        queue->front->prev = node;
    else
        queue->back = node;
    
    queue->front = node;
    return 1;
}

void *dequeue(queue_t *queue) {
    queue_node_t *node;
    void *ptr;
    
    if (!queue || !queue->front)
        return NULL;
    
    node = queue->front;
    ptr = node->ptr;
    
    queue->front = node->next;
    if (queue->front)
        queue->front->prev = NULL;
    else
        queue->back = NULL;
    
    free(node);
    return ptr;
}

/* Graph functions - simplified */
graph_t *graph_create(void) {
    graph_t *graph = malloc(sizeof(graph_t));
    if (graph) {
        graph->vertices = NULL;
        graph->nb_vertices = 0;
    }
    return graph;
}

vertex_t *graph_add_vertex(graph_t *graph, char const *str, int x, int y) {
    vertex_t *vertex;
    
    (void)x;  /* Suppress unused parameter warning */
    (void)y;  /* Suppress unused parameter warning */
    
    if (!graph || !str)
        return NULL;
    
    vertex = malloc(sizeof(vertex_t));
    if (!vertex)
        return NULL;
    
    vertex->content = strdup(str);
    if (!vertex->content) {
        free(vertex);
        return NULL;
    }
    
    vertex->edges = NULL;
    vertex->next = graph->vertices;
    graph->vertices = vertex;
    graph->nb_vertices++;
    
    return vertex;
}

int graph_add_edge(graph_t *graph, char const *src, char const *dest,
                   int weight, edge_type_t type) {
    vertex_t *src_vertex = NULL, *dest_vertex = NULL;
    vertex_t *temp;
    edge_t *edge;
    
    if (!graph || !src || !dest)
        return 0;
    
    /* Find source and destination vertices */
    temp = graph->vertices;
    while (temp && (!src_vertex || !dest_vertex)) {
        if (strcmp(temp->content, src) == 0)
            src_vertex = temp;
        if (strcmp(temp->content, dest) == 0)
            dest_vertex = temp;
        temp = temp->next;
    }
    
    if (!src_vertex || !dest_vertex)
        return 0;
    
    /* Add edge from src to dest */
    edge = malloc(sizeof(edge_t));
    if (!edge)
        return 0;
    
    edge->dest = dest_vertex;
    edge->weight = weight;
    edge->next = src_vertex->edges;
    src_vertex->edges = edge;
    
    /* If bidirectional, add reverse edge */
    if (type == BIDIRECTIONAL) {
        edge = malloc(sizeof(edge_t));
        if (!edge)
            return 1; /* At least one edge was added */
        
        edge->dest = src_vertex;
        edge->weight = weight;
        edge->next = dest_vertex->edges;
        dest_vertex->edges = edge;
    }
    
    return 1;
}

void graph_delete(graph_t *graph) {
    vertex_t *vertex, *next_vertex;
    edge_t *edge, *next_edge;
    
    if (!graph)
        return;
    
    vertex = graph->vertices;
    while (vertex) {
        next_vertex = vertex->next;
        
        edge = vertex->edges;
        while (edge) {
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

/* Dijkstra algorithm implementation */
static int compareWeights(const void *param1, const void *param2) {
    const dijkstra_vertex_t *D_vert1 = param1;
    const dijkstra_vertex_t *D_vert2 = param2;

    if (D_vert1->cml_weight > D_vert2->cml_weight)
        return 1;
    if (D_vert1->cml_weight < D_vert2->cml_weight)
        return -1;
    return 0;
}

static queue_t *pathFromDijkstraQueue(dijkstra_vertex_t *d_queue, 
                                      size_t nb_vertices, size_t target_i) {
    queue_t *path = NULL;
    char *via_name = NULL;
    size_t i;
    vertex_t *current_vertex;

    if (!d_queue)
        return NULL;

    path = queue_create();
    if (!path)
        return NULL;

    /* Start from target and work backwards to start */
    i = target_i;
    while (1) {
        via_name = strdup(d_queue[i].vertex->content);
        if (!via_name || !queue_push_front(path, via_name)) {
            if (via_name) free(via_name);
            while (path->front)
                free(dequeue(path));
            free(path);
            return NULL;
        }

        /* Check if we reached the start vertex */
        if (d_queue[i].vertex == d_queue[0].vertex || 
            d_queue[i].path_via == NULL)
            break;

        /* Find the previous vertex in the path */
        current_vertex = d_queue[i].path_via;
        for (i = 0; i < nb_vertices; i++) {
            if (d_queue[i].vertex == current_vertex)
                break;
        }
        
        if (i >= nb_vertices) { /* Couldn't find previous vertex */
            while (path->front)
                free(dequeue(path));
            free(path);
            return NULL;
        }
    }

    return path;
}

static void assessEdges(dijkstra_vertex_t *d_queue, size_t nb_vertices,
                        size_t dq_head_i) {
    dijkstra_vertex_t *dq_head = &d_queue[dq_head_i];
    edge_t *edge = NULL;
    size_t j;
    size_t new_weight;

    if (!dq_head)
        return;

    /* Check all edges from current vertex */
    for (edge = dq_head->vertex->edges; edge; edge = edge->next) {
        /* Skip if going back to previous vertex */
        if (dq_head->path_via && edge->dest == dq_head->path_via)
            continue;

        /* Find destination vertex in the queue */
        for (j = dq_head_i; j < nb_vertices; j++) {
            if (d_queue[j].vertex == edge->dest) {
                /* Check for potential overflow */
                if (dq_head->cml_weight > ULONG_MAX - edge->weight)
                    continue;

                new_weight = dq_head->cml_weight + edge->weight;
                
                /* If found a shorter path, update it */
                if (new_weight < d_queue[j].cml_weight) {
                    d_queue[j].cml_weight = new_weight;
                    d_queue[j].path_via = dq_head->vertex;
                }
                break;
            }
        }
    }

    /* Sort the remaining vertices */
    if (dq_head_i + 1 < nb_vertices) {
        qsort(d_queue + dq_head_i + 1, nb_vertices - dq_head_i - 1,
              sizeof(dijkstra_vertex_t), compareWeights);
    }
}

static int dijkstraGraph(dijkstra_vertex_t *d_queue, size_t nb_vertices,
                         const vertex_t *start, const vertex_t *target,
                         size_t *target_i) {
    size_t dq_head_i = 0;
    
    if (!d_queue || !start || !target || !target_i)
        return 1;

    while (dq_head_i < nb_vertices) {
        dijkstra_vertex_t *dq_head = &d_queue[dq_head_i];
        
        /* Print as required by the example */
        printf("Checking %s, distance from %s is %lu\n",
               dq_head->vertex->content, start->content, 
               (unsigned long)dq_head->cml_weight);

        /* Check if we reached target */
        if (dq_head->vertex == target || 
            strcmp(dq_head->vertex->content, target->content) == 0) {
            *target_i = dq_head_i;
            return 0;
        }

        /* If current distance is infinity, no more reachable vertices */
        if (dq_head->cml_weight == ULONG_MAX)
            break;

        /* Update distances for neighboring vertices */
        assessEdges(d_queue, nb_vertices, dq_head_i);
        
        dq_head_i++;
    }

    return 1; /* Target not found */
}

queue_t *dijkstra_graph(graph_t *graph, vertex_t const *start,
                        vertex_t const *target) {
    dijkstra_vertex_t *dijkstra_queue = NULL;
    vertex_t *temp_v = NULL;
    size_t i, target_i;
    queue_t *path = NULL;

    if (!graph || !graph->nb_vertices || !graph->vertices ||
        !start || !target)
        return NULL;

    /* Allocate and initialize Dijkstra queue */
    dijkstra_queue = malloc(sizeof(dijkstra_vertex_t) * graph->nb_vertices);
    if (!dijkstra_queue)
        return NULL;

    /* Initialize all vertices */
    temp_v = graph->vertices;
    for (i = 0; i < graph->nb_vertices; i++) {
        dijkstra_queue[i].vertex = temp_v;
        dijkstra_queue[i].path_via = NULL;

        /* Start vertex has distance 0, others infinite */
        if (temp_v == start || 
            strcmp(temp_v->content, start->content) == 0) {
            dijkstra_queue[i].cml_weight = 0;
        } else {
            dijkstra_queue[i].cml_weight = ULONG_MAX;
        }
        temp_v = temp_v->next;
    }

    /* Initial sort by distance */
    qsort(dijkstra_queue, graph->nb_vertices,
          sizeof(dijkstra_vertex_t), compareWeights);

    /* Run Dijkstra's algorithm */
    if (dijkstraGraph(dijkstra_queue, graph->nb_vertices,
                      start, target, &target_i) != 0) {
        free(dijkstra_queue);
        return NULL;
    }

    /* Reconstruct the path */
    path = pathFromDijkstraQueue(dijkstra_queue, graph->nb_vertices, target_i);
    free(dijkstra_queue);
    
    return path;
}

/* Test function */
static void print_free_path(queue_t *path) {
    printf("Path found:\n");
    while (path->front) {
        char *city = dequeue(path);
        printf(" %s\n", city);
        free(city);
    }
    free(path);
}

int main(void) {
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
    graph_add_edge(graph, "San Francisco", "Los Angeles", 347, BIDIRECTIONAL);
    graph_add_edge(graph, "San Francisco", "Las Vegas", 417, BIDIRECTIONAL);
    graph_add_edge(graph, "Los Angeles", "Las Vegas", 228, BIDIRECTIONAL);
    graph_add_edge(graph, "San Francisco", "Seattle", 680, BIDIRECTIONAL);
    graph_add_edge(graph, "Seattle", "Chicago", 1734, BIDIRECTIONAL);
    graph_add_edge(graph, "Chicago", "Washington", 594, BIDIRECTIONAL);
    graph_add_edge(graph, "Washington", "New York", 203, BIDIRECTIONAL);
    graph_add_edge(graph, "Las Vegas", "Houston", 1227, BIDIRECTIONAL);
    graph_add_edge(graph, "Houston", "Nashville", 666, BIDIRECTIONAL);
    graph_add_edge(graph, "Nashville", "Washington", 566, BIDIRECTIONAL);
    graph_add_edge(graph, "Nashville", "Miami", 818, BIDIRECTIONAL);

    path = dijkstra_graph(graph, start, target);
    if (!path) {
        fprintf(stderr, "Failed to retrieve path\n");
        return 1;
    }

    print_free_path(path);
    graph_delete(graph);

    return 0;
}