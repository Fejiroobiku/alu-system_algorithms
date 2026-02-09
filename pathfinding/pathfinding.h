#ifndef PATHFINDING_H
#define PATHFINDING_H

/* queue_t */
#include "queues.h"
/* graph_t */
#include "graphs.h"


/**
 * struct point_s - Structure storing coordinates
 *
 * @x: X coordinate
 * @y: Y coordinate
 */
typedef struct point_s
{
	int x;
	int y;
} point_t;


/* 2-dijkstra_graph.c */
/**
  * struct dijkstra_vertex_s - used to implement a Dijkstra priority queue as
  *   a sortable array
  *
  * @vertex: vertex in graph to be traversed
  * @cml_weight: current cumulative weight of vertex, sum of weights of all
  *   edges along currently defined optimal path from start
  * @path_via: previous vertex in currently defined optimal path from start
  */
typedef struct dijkstra_vertex_s
{
	vertex_t *vertex;
	size_t cml_weight;
	vertex_t *path_via;
} dijkstra_vertex_t;


/* 100-a_star_graph.c */
/**
  * struct a_star_vertex_s - used to implement a Dijkstra priority queue as
  *   a sortable array
  *
  * @vertex: vertex in graph to be traversed
  * @cml_weight: current cumulative weight of vertex, sum of weights of all
  *   edges along currently defined optimal path from start
  * @heuristic: added to cml_weight to set order of priority queue; in this
  *   case represents the Euclidean distance from vertex to target
  * @path_via: previous vertex in currently defined optimal path from start
  */
typedef struct a_star_vertex_s
{
	vertex_t *vertex;
	size_t cml_weight;
	size_t heuristic;
	vertex_t *path_via;
} a_star_vertex_t;


/* 0-backtracking_array.c */
queue_t *backtracking_array(char **map, int rows, int cols,
			    point_t const *start, point_t const *target);

/* 1-backtracking_graph.c */
queue_t *backtracking_graph(graph_t *graph, vertex_t const *start,
			    vertex_t const *target);

/* 2-dijkstra_graph.c */
queue_t *dijkstra_graph(graph_t *graph, vertex_t const *start,
			vertex_t const *target);

/* euclideanDist.c */
size_t euclideanDist(const vertex_t *vertex, const vertex_t *target);

/* 100-a_star_graph.c */
queue_t *a_star_graph(graph_t *graph, vertex_t const *start,
		      vertex_t const *target);


#endif /* PATHFINDING_H */