#include <stdlib.h>
#include "graphs.h"

/**
 * graph_create - Allocates and initializes a graph structure
 *
 * Return: Pointer to the newly created graph, or NULL on failure
 */
graph_t *graph_create(void)
{
	graph_t *graph;

	graph = calloc(1, sizeof(graph_t));
	if (!graph)
		return (NULL);

	return (graph);
}