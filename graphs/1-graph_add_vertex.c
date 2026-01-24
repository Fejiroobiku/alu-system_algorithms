#include <stdlib.h>
#include <string.h>
#include "graphs.h"

/**
 * content_exists - Checks if content already exists in the vertex list
 * @head: Pointer to the first vertex
 * @str: String to search for
 *
 * Return: 1 if found, 0 otherwise
 */
static int content_exists(const vertex_t *head, const char *str)
{
	const vertex_t *tmp = head;

	while (tmp)
	{
		if (strcmp(tmp->content, str) == 0)
			return (1);
		tmp = tmp->next;
	}
	return (0);
}

/**
 * create_vertex - Allocates and initializes a new vertex
 * @str: Content string (will be duplicated)
 * @index: Index to assign
 *
 * Return: New vertex pointer, or NULL on failure
 */
static vertex_t *create_vertex(const char *str, size_t index)
{
	vertex_t *v = calloc(1, sizeof(vertex_t));
	char *copy;

	if (!v)
		return (NULL);

	copy = strdup(str);
	if (!copy)
	{
		free(v);
		return (NULL);
	}

	v->content = copy;
	v->index = index;
	v->nb_edges = 0;
	v->edges = NULL;
	v->next = NULL;

	return (v);
}

/**
 * append_vertex - Appends vertex to the end of the list
 * @graph: Graph to modify
 * @v: Vertex to append
 */
static void append_vertex(graph_t *graph, vertex_t *v)
{
	vertex_t *tmp;

	if (!graph->vertices)
	{
		graph->vertices = v;
	}
	else
	{
		tmp = graph->vertices;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = v;
	}
	graph->nb_vertices++;
}

/**
 * graph_add_vertex - Adds a vertex to an existing graph
 * @graph: Pointer to the graph
 * @str: String to store in the new vertex
 *
 * Return: Pointer to the created vertex, or NULL on failure
 */
vertex_t *graph_add_vertex(graph_t *graph, const char *str)
{
	vertex_t *v;

	if (!graph || !str)
		return (NULL);

	if (content_exists(graph->vertices, str))
		return (NULL);

	v = create_vertex(str, graph->nb_vertices);
	if (!v)
		return (NULL);

	append_vertex(graph, v);

	return (v);
}