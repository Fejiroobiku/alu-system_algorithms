#include "huffman.h"
#include <stdlib.h>

/**
 * huffman_tree - builds a Huffman tree from characters and their frequencies
 * @data: array of characters
 * @freq: array of frequencies (same size as data)
 * @size: number of elements in data and freq
 *
 * Return: pointer to root of Huffman tree, or NULL on failure
 */
binary_tree_node_t *huffman_tree(char *data, size_t *freq, size_t size)
{
	heap_t *q;
	binary_tree_node_t *root;

	if (!data || !freq || size == 0)
		return (NULL);

	/* create priority queue (min heap) */
	q = huffman_priority_queue(data, freq, size);
	if (!q)
		return (NULL);

	/* repeatedly extract two smallest nodes and merge until one node remains */
	while (q->size > 1)
		huffman_extract_and_insert(q);

	/* the remaining node in the heap is the root of the Huffman tree */
	root = (binary_tree_node_t *)q->root->data;

	/* free the heap structure (not the node itself) */
	free(q->root);
	free(q);

	return (root);
}