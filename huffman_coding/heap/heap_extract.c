#include "heap.h"
#include <stdlib.h>

/**
 * heapify - restores heap property from root downwards
 * @root: node_t root of heap subtree
 * @data_cmp: comparison function for node data
 */
static void heapify(node_t *root, int (*data_cmp)(void *, void *))
{
	if (!root || !root->left || !data_cmp)
		return;

	if (root->right && data_cmp(root->left->data, root->right->data) > 0)
	{
		if (data_cmp(root->data, root->right->data) >= 0)
			DATASWAP(root, root->right);
		heapify(root->right, data_cmp);
	}
	else
	{
		if (data_cmp(root->data, root->left->data) >= 0)
			DATASWAP(root, root->left);
		heapify(root->left, data_cmp);
	}
}

/**
 * heap_to_array - converts heap tree to array while calculating size
 * @array: buffer to store heap nodes
 * @root: root node of heap subtree
 * @i: index in array
 * Return: size_t size of heap
 */
static size_t heap_to_array(node_t **array, node_t *root, size_t i)
{
	if (!root || !array)
		return (0);

	array[i] = root;

	return (1 + heap_to_array(array, root->left, 2 * i + 1)
			+ heap_to_array(array, root->right, 2 * i + 2));
}

/**
 * heap_extract - extracts the root value of a Min Binary Heap
 * @heap: heap_t pointer
 * Return: pointer to data stored in heap's root or NULL
 */
void *heap_extract(heap_t *heap)
{
	node_t *a[32], *last;
	void *data;

	if (!heap || !heap->root)
		return (NULL);

	if (heap->size != heap_to_array(a, heap->root, 0))
		return (NULL);

	data = heap->root->data;
	last = a[heap->size - 1];

	if (heap->root == last)
		heap->root = NULL;
	else
	{
		heap->root->data = last->data;

		if (last->parent->left == last)
			last->parent->left = NULL;
		else
			last->parent->right = NULL;

		heapify(heap->root, heap->data_cmp);
	}

	free(last);
	heap->size--;

	return (data);
}