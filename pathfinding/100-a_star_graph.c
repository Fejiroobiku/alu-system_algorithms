#include "pathfinding.h"
/* malloc free qsort */
#include <stdlib.h>
/* printf */
#include <stdio.h>
/* strcmp strdup */
#include <string.h>
/* ULONG_MAX */
#include <limits.h>
/* euclideanDist */
#include "euclideanDist.c"


/**
  * pathFromAStarQueue - once an A* priority queue has been populated
  *   by traversing the graph, this function works backwards through the
  *   priority queue to build a solution queue by following the path_via
  *   pointers from target to start
  *
  * @d_queue: finished A* priority queue
  * @target_i: index of target vertex in priority queue
  * Return: queue containing the content strings from each node in the graph
  *   from start to target, or NULL on failure
  */
queue_t *pathFromAStarQueue(a_star_vertex_t *d_queue,
			    size_t target_i)
{
	queue_t *path = NULL;
	vertex_t *via = NULL;
	char *via_name = NULL;
	long int i;

	if (!d_queue)
		return (NULL);

	path = queue_create();
	if (!path)
		return (NULL);

	via_name = strdup(d_queue[target_i].vertex->content);
	queue_push_front(path, (void *)via_name);
	via = d_queue[target_i].path_via;

	for (i = target_i; i >= 0; i--)
	{
		if (d_queue[i].vertex == via)
		{
			via_name = strdup(d_queue[i].vertex->content);
			queue_push_front(path, (void *)via_name);
			via = d_queue[i].path_via;
		}
	}

	if (via != NULL) /* no contiguous path from target to start */
	{
		while (path->front)
			free(dequeue(path));
		queue_delete(path);
		path = NULL;
	}

	return (path);
}


/**
  * compareWeightPlusHeuristic - intended for use with qsort(), sorts members
  *   of the A* priority queue by their cumulative weights (made into stable
  *   sort by using pointer values as tiebreaker)
  *
  * @param1: void * due to function prototype expected by qsort(),
  *   expected to be castable to dijkstra_vertex *
  * @param2: void * due to function prototype expected by qsort(),
  *   expected to be castable to dijkstra_vertex *
  * Return: 1 if param1 should be ordered first, -1 if param2 is first,
  *   or 0 if they are the same
  */
int compareWeightPlusHeuristic(const void *param1, const void *param2)
{
	a_star_vertex_t *Astar_v1 = NULL, *Astar_v2 = NULL;

	if (!param1 && !param2)
		return (0);
	if (param1 && !param2)
		return (1);
	if (!param1 && param2)
		return (-1);

	Astar_v1 = (a_star_vertex_t *)param1;
	Astar_v2 = (a_star_vertex_t *)param2;

	/* stabilizes sort by using pointer values in array as tiebreaker */
	if (Astar_v1->cml_weight + Astar_v1->heuristic >
	    Astar_v2->cml_weight + Astar_v2->heuristic)
		return (1);
	if (Astar_v1->cml_weight + Astar_v1->heuristic <
	    Astar_v2->cml_weight + Astar_v2->heuristic)
		return (-1);
	else if (param1 > param2)
		return (1);
	else if (param1 < param2)
		return (-1);

	return (0);
}


/**
  * assessEdges - finds smallest cumulative weights and optimal previous vertex
  *   for each edge of the current head of the priority queue
  *
  * @as_queue: array of structs containing current vertex, cumulative weight,
  *   previous vertex in current optimal route
  * @nb_vertices: total amount of vertices in graph
  * @asq_head_i: index in d_queue that marks the current head of the priority
  *   queue; any indexes lower than this in the array have finished assessment
  */
void assessEdges(a_star_vertex_t *as_queue, size_t nb_vertices,
		 size_t asq_head_i)
{
	a_star_vertex_t asq_head;
	edge_t *temp_e = NULL;
	size_t i;

	if (!as_queue)
		return;

	asq_head = as_queue[asq_head_i];

	for (temp_e = asq_head.vertex->edges; temp_e; temp_e = temp_e->next)
	{
		if (asq_head.path_via && strcmp(temp_e->dest->content,
					       asq_head.path_via->content) == 0)
			continue;

		for (i = asq_head_i; i < nb_vertices; i++)
		{
			if (strcmp(temp_e->dest->content,
				   as_queue[i].vertex->content) == 0)
			{
				if (asq_head.cml_weight + temp_e->weight <
				    as_queue[i].cml_weight)
				{
					as_queue[i].cml_weight =
						asq_head.cml_weight +
						temp_e->weight;

					as_queue[i].path_via = asq_head.vertex;
				}
			}
		}
	}

	asq_head_i++;

	qsort((void *)(as_queue + asq_head_i), nb_vertices - asq_head_i,
	      sizeof(a_star_vertex_t), compareWeightPlusHeuristic);
}


/**
  * aStarGraph - updates an array-based priority queue to find smallest
  *   cumulative weights and optimal previous vertex for each vertex in a graph
  *
  * @as_queue: array of structs containing current vertex, cumulative weight,
  *   previous vertex in current optimal route
  * @nb_vertices: total amount of vertices in graph
  * @start: pointer to starting vertex
  * @target: pointer to target vertex
  * @asq_head_i: index in d_queue that marks the current head of the priority
  *   queue; any indexes lower than this in the array have finished assessment
  * @target_i: once assessment of the graph is complete and the target is
  *   found, this is updated to the final index of the target vertex in d_queue
  * Return: 0 on success, 1 on failure or if target not found
  */
int aStarGraph(a_star_vertex_t *as_queue, size_t nb_vertices,
	       const vertex_t *start, const vertex_t *target,
	       size_t asq_head_i, size_t *target_i)
{
	a_star_vertex_t asq_head;

	if (!as_queue || !start || !target || !target_i)
		return (1);

	asq_head = as_queue[asq_head_i];
	printf("Checking %s, distance to %s is %lu\n",
	       asq_head.vertex->content, target->content, asq_head.heuristic);

	assessEdges(as_queue, nb_vertices, asq_head_i);

	if (strcmp(target->content, asq_head.vertex->content) == 0)
	{
		*target_i = asq_head_i;
		return (0);
	}

	/* no more to assess */
	if (asq_head_i == nb_vertices - 1)
		return (1);

	/* no more viable paths remain */
	if (as_queue[asq_head_i + 1].cml_weight == ULONG_MAX ||
	    as_queue[asq_head_i + 1].path_via == NULL)
		return (1);

	return (aStarGraph(as_queue, nb_vertices, start, target,
			     asq_head_i + 1, target_i));
}


/**
  * a_star_graph - searches for the shortest and fastest path from a starting
  *   point to a target point in a graph, using the A* algorithm with Euclidean
  *   distance between vertex and target as the additional heuristic
  *
  * @graph: pointer to the graph to traverse
  * @start: pointer to starting vertex
  * @target: pointer to target vertex
  * Return: returns a queue in which each node is a `char *` corresponding to a
  *   vertex, forming a path from start to target, or NULL on failure
  */
queue_t *a_star_graph(graph_t *graph, vertex_t const *start,
		      vertex_t const *target)
{
	a_star_vertex_t *a_star_queue = NULL;
	vertex_t *temp_v = NULL;
	size_t i, target_i;
	queue_t *path = NULL;

	if (!graph || !graph->nb_vertices || !graph->vertices ||
	    !start || !target)
		return (NULL);

	a_star_queue = malloc(sizeof(a_star_vertex_t) * graph->nb_vertices);
	if (!a_star_queue)
		return (NULL);

	for (i = 0, temp_v = graph->vertices; i < graph->nb_vertices;
	     i++, temp_v = temp_v->next)
	{
		a_star_queue[i].vertex = temp_v;
		a_star_queue[i].heuristic = euclideanDist(temp_v, target);

		if (strcmp(start->content, temp_v->content) == 0)
			a_star_queue[i].cml_weight = 0;
		else
			a_star_queue[i].cml_weight = ULONG_MAX -
				a_star_queue[i].heuristic;

		a_star_queue[i].path_via = NULL;
	}
	qsort((void *)a_star_queue, graph->nb_vertices,
	      sizeof(a_star_vertex_t), compareWeightPlusHeuristic);

	if (aStarGraph(a_star_queue, graph->nb_vertices,
			  start, target, 0, &target_i) != 0)
	{
		free(a_star_queue);
		return (NULL);
	}
	/* assemble path backwards from a_star_queue */
	path = pathFromAStarQueue(a_star_queue, target_i);
	free(a_star_queue);
	return (path);
}