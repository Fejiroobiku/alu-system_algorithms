#include <stdlib.h>
#include "graphs.h"

/**
 * bfs_init - Initialize BFS helper arrays
 * @graph: Pointer to the graph
 * @queue: Address of queue
 * @depths: Address of depths array
 * @visited: Address of visited array
 *
 * Return: 0 on success, 1 on failure
 */
static int bfs_init(const graph_t *graph, vertex_t ***queue,
		size_t **depths, int **visited)
{
	*queue = calloc(graph->nb_vertices, sizeof(vertex_t *));
	*depths = calloc(graph->nb_vertices, sizeof(size_t));
	*visited = calloc(graph->nb_vertices, sizeof(int));

	if (!*queue || !*depths || !*visited)
		return (1);
	return (0);
}

/**
 * bfs_free - Free BFS helper arrays
 * @queue: BFS queue
 * @depths: Depths array
 * @visited: Visited array
 */
static void bfs_free(vertex_t **queue, size_t *depths, int *visited)
{
	free(queue);
	free(depths);
	free(visited);
}

/**
 * bfs_process - Perform the BFS traversal
 * @graph: Pointer to the graph
 * @action: Action function
 * @queue: BFS queue
 * @depths: Depths array
 *
 * Return: Maximum depth reached
 */
static size_t bfs_process(const graph_t *graph,
		void (*action)(const vertex_t *, size_t),
		vertex_t **queue, size_t *depths)
{
	size_t front = 0, rear = 0, max_depth = 0, depth;
	vertex_t *v;
	edge_t *edge;
	int *visited = calloc(graph->nb_vertices, sizeof(int));

	if (!visited)
		return (0);

	queue[rear] = graph->vertices;
	depths[rear++] = 0;
	visited[graph->vertices->index] = 1;

	while (front < rear)
	{
		v = queue[front];
		depth = depths[front++];
		action(v, depth);

		if (depth > max_depth)
			max_depth = depth;

		for (edge = v->edges; edge; edge = edge->next)
		{
			if (!visited[edge->dest->index])
			{
				visited[edge->dest->index] = 1;
				queue[rear] = edge->dest;
				depths[rear++] = depth + 1;
			}
		}
	}

	free(visited);
	return (max_depth);
}

/**
 * breadth_first_traverse - Traverses a graph using BFS
 * @graph: Pointer to the graph to traverse
 * @action: Function to call on each visited vertex
 *
 * Return: Maximum depth reached, or 0 on failure
 */
size_t breadth_first_traverse(const graph_t *graph,
		void (*action)(const vertex_t *, size_t))
{
	vertex_t **queue;
	size_t *depths;
	int *visited;
	size_t max_depth;

	if (!graph || !graph->vertices || !action)
		return (0);

	if (bfs_init(graph, &queue, &depths, &visited))
	{
		bfs_free(queue, depths, visited);
		return (0);
	}

	max_depth = bfs_process(graph, action, queue, depths);
	bfs_free(queue, depths, visited);

	return (max_depth);
}