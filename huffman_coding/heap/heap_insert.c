#include "heap.h"

/**
 * heapify - restores heap property from leaf node of subtree
 * @node: node_t leaf node of heap tree
 * @data_cmp: comparison function over generic data pointers
 * Return: node_t pointer to sifted up node
 */
static node_t *heapify(node_t *node, int (*data_cmp)(void *, void *))
{
	if (!node || !data_cmp)
		return (NULL);

	while (node->parent && data_cmp(node->data, node->parent->data) < 0)
	{
		DATASWAP(node, node->parent);
		node = node->parent;
	}

	return (node);
}

/**
 * heap_insert - inserts a value in a binary heap
 *                  (uses static reference array)
 * @heap: pointer to heap_t where we'll insert node
 * @data: generic pointer to data for new node
 * Return: node_t pointer to the newly inserted node
 */
node_t *heap_insert(heap_t *heap, void *data)
{
	static node_t *harray[32];
	node_t *new, *parent;

	if (!heap || !data)
		return (NULL);

	if (heap->size == 0)
	{
		harray[heap->size++] = binary_tree_node(NULL, data);
		heap->root = harray[0];
		return (heap->root);
	}

	/* find parent of new node */
	parent = harray[(heap->size - 1) / 2];

	/* insert left or right */
	harray[heap->size++] = (!parent->left)
		? (parent->left = binary_tree_node(parent, data))
		: (parent->right = binary_tree_node(parent, data));

	/* restore heap property */
	new = heapify(harray[heap->size - 1], heap->data_cmp);

	return (new);
}