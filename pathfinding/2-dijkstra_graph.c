#include "pathfinding.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

/**
 * compareWeights - qsort comparator for Dijkstra queue
 * @param1: First vertex
 * @param2: Second vertex
 * Return: Comparison result
 */
static int compareWeights(const void *param1, const void *param2)
{
    const dijkstra_vertex_t *D_vert1 = param1;
    const dijkstra_vertex_t *D_vert2 = param2;

    if (D_vert1->cml_weight > D_vert2->cml_weight)
        return (1);
    if (D_vert1->cml_weight < D_vert2->cml_weight)
        return (-1);
    return (0);
}

/**
 * pathFromDijkstraQueue - Builds path from Dijkstra results
 * @d_queue: Dijkstra queue
 * @nb_vertices: Total vertices
 * @target_i: Target index
 * Return: Path queue
 */
static queue_t *pathFromDijkstraQueue(dijkstra_vertex_t *d_queue, 
                                      size_t nb_vertices, size_t target_i)
{
    queue_t *path = NULL;
    char *via_name = NULL;
    size_t i;
    vertex_t *current_vertex;

    if (!d_queue)
        return (NULL);

    path = queue_create();
    if (!path)
        return (NULL);

    /* Start from target and work backwards to start */
    i = target_i;
    while (1)
    {
        via_name = strdup(d_queue[i].vertex->content);
        if (!via_name || !queue_push_front(path, via_name))
        {
            if (via_name) free(via_name);
            while (path->front)
                free(dequeue(path));
            free(path);
            return (NULL);
        }

        /* Check if we reached the start vertex */
        if (d_queue[i].vertex == d_queue[0].vertex || 
            d_queue[i].path_via == NULL)
            break;

        /* Find the previous vertex in the path */
        current_vertex = d_queue[i].path_via;
        for (i = 0; i < nb_vertices; i++)
        {
            if (d_queue[i].vertex == current_vertex)
                break;
        }
        
        if (i >= nb_vertices) /* Couldn't find previous vertex */
        {
            while (path->front)
                free(dequeue(path));
            free(path);
            return (NULL);
        }
    }

    return (path);
}

/**
 * assessEdges - Updates distances for edges from current vertex
 * @d_queue: Dijkstra queue
 * @nb_vertices: Number of vertices
 * @dq_head_i: Current head index
 */
static void assessEdges(dijkstra_vertex_t *d_queue, size_t nb_vertices,
                        size_t dq_head_i)
{
    dijkstra_vertex_t *dq_head = &d_queue[dq_head_i];
    edge_t *edge = NULL;
    size_t j;
    size_t new_weight;

    if (!dq_head)
        return;

    /* Check all edges from current vertex */
    for (edge = dq_head->vertex->edges; edge; edge = edge->next)
    {
        /* Skip if going back to previous vertex */
        if (dq_head->path_via && edge->dest == dq_head->path_via)
            continue;

        /* Find destination vertex in the queue - CHECK ALL VERTICES */
        for (j = 0; j < nb_vertices; j++)  /* FIXED: was (j = dq_head_i) */
        {
            if (d_queue[j].vertex == edge->dest)
            {
                /* Check for potential overflow */
                if (dq_head->cml_weight > ULONG_MAX - edge->weight)
                    continue;

                new_weight = dq_head->cml_weight + edge->weight;
                
                /* If found a shorter path, update it */
                if (new_weight < d_queue[j].cml_weight)
                {
                    d_queue[j].cml_weight = new_weight;
                    d_queue[j].path_via = dq_head->vertex;
                }
                break;
            }
        }
    }

    /* Sort the remaining vertices */
    if (dq_head_i + 1 < nb_vertices)
    {
        qsort(d_queue + dq_head_i + 1, nb_vertices - dq_head_i - 1,
              sizeof(dijkstra_vertex_t), compareWeights);
    }
}

/**
 * dijkstraGraph - Main Dijkstra algorithm
 * @d_queue: Dijkstra queue
 * @nb_vertices: Number of vertices
 * @start: Start vertex
 * @target: Target vertex
 * @target_i: Output parameter for target index
 * Return: 0 on success, 1 on failure
 */
static int dijkstraGraph(dijkstra_vertex_t *d_queue, size_t nb_vertices,
                         const vertex_t *start, const vertex_t *target,
                         size_t *target_i)
{
    size_t dq_head_i = 0;
    
    if (!d_queue || !start || !target || !target_i)
        return (1);

    while (dq_head_i < nb_vertices)
    {
        dijkstra_vertex_t *dq_head = &d_queue[dq_head_i];
        
        /* Print as required by the example */
        printf("Checking %s, distance from %s is %lu\n",
               dq_head->vertex->content, start->content, 
               (unsigned long)dq_head->cml_weight);

        /* Check if we reached target */
        if (dq_head->vertex == target || 
            strcmp(dq_head->vertex->content, target->content) == 0)
        {
            *target_i = dq_head_i;
            return (0);
        }

        /* If current distance is infinity, no more reachable vertices */
        if (dq_head->cml_weight == ULONG_MAX)
            break;

        /* Update distances for neighboring vertices */
        assessEdges(d_queue, nb_vertices, dq_head_i);
        
        dq_head_i++;
    }

    return (1); /* Target not found */
}

/**
 * dijkstra_graph - Finds shortest path using Dijkstra's algorithm
 * @graph: Graph to traverse
 * @start: Starting vertex
 * @target: Target vertex
 * Return: Queue containing path
 */
queue_t *dijkstra_graph(graph_t *graph, vertex_t const *start,
                        vertex_t const *target)
{
    dijkstra_vertex_t *dijkstra_queue = NULL;
    vertex_t *temp_v = NULL;
    size_t i, target_i;
    queue_t *path = NULL;

    if (!graph || !graph->nb_vertices || !graph->vertices ||
        !start || !target)
        return (NULL);

    /* Allocate and initialize Dijkstra queue */
    dijkstra_queue = malloc(sizeof(dijkstra_vertex_t) * graph->nb_vertices);
    if (!dijkstra_queue)
        return (NULL);

    /* Initialize all vertices */
    for (i = 0, temp_v = graph->vertices; i < graph->nb_vertices;
         i++, temp_v = temp_v->next)
    {
        dijkstra_queue[i].vertex = temp_v;
        dijkstra_queue[i].path_via = NULL;

        /* Start vertex has distance 0, others infinite */
        if (temp_v == start || 
            strcmp(temp_v->content, start->content) == 0)
        {
            dijkstra_queue[i].cml_weight = 0;
        }
        else
        {
            dijkstra_queue[i].cml_weight = ULONG_MAX;
        }
    }

    /* Initial sort by distance */
    qsort(dijkstra_queue, graph->nb_vertices,
          sizeof(dijkstra_vertex_t), compareWeights);

    /* Run Dijkstra's algorithm */
    if (dijkstraGraph(dijkstra_queue, graph->nb_vertices,
                      start, target, &target_i) != 0)
    {
        free(dijkstra_queue);
        return (NULL);
    }

    /* Reconstruct the path */
    path = pathFromDijkstraQueue(dijkstra_queue, graph->nb_vertices, target_i);
    free(dijkstra_queue);
    
    return (path);
}