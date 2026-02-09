#include "pathfinding.h"
/* fprintf */
#include <stdio.h>
/* sqrt */
#include <math.h>

#define ABS(x)    ((x) > 0 ? (x) : -(x))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))


/**
  * euclideanDist - calculates the distance between two point in a coordinate
  *   grid, defined as the hypoteneuse of the differences in their x and y
  *   values
  *
  * @vertex: pointer to starting vertex
  * @target: pointer to target vertex
  * Return: distance between the two points in grid units, rounded down to
  *   nearest integer, or 0 on failure (with message)
  */
size_t euclideanDist(const vertex_t *vertex, const vertex_t *target)
{
	int dx, dy;
	double hyp;

	if (!vertex || !target)
	{
		fprintf(stderr, "euclideanDist: NULL parameters\n");
		return (0);
	}

	if ((vertex->x >= 0 && target->x >= 0) ||
	    (vertex->x < 0 && target->x < 0))
		dx = MAX(ABS(vertex->x), ABS(target->x)) -
			MIN(ABS(vertex->x), ABS(target->x));
	else
		dx = ABS(vertex->x) + ABS(target->x);

	if ((vertex->y >= 0 && target->y >= 0) ||
	    (vertex->y < 0 && target->y < 0))
		dy = MAX(ABS(vertex->y), ABS(target->y)) -
			MIN(ABS(vertex->y), ABS(target->y));
	else
		dy = ABS(vertex->y) + ABS(target->y);

	hyp = sqrt((dx * dx) + (dy * dy));
	if (!isfinite(hyp))
	{
		fprintf(stderr, "euclideanDist: square root is not finite\n");
		return (0);
	}

	return ((size_t)hyp);
}