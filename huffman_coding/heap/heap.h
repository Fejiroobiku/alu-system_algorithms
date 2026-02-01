#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>

/**
 * struct binary_tree_node_s - Binary tree node
 *
 * @data: Data stored in the node
 * @left: Pointer to the left child
 * @right: Pointer to the right child
 * @parent: Pointer to the parent node
 */
typedef struct binary_tree_node_s
{
	void *data;
	struct binary_tree_node_s *left;
	struct binary_tree_node_s *right;
	struct binary_tree_node_s *parent;
} binary_tree_node_t;

typedef binary_tree_node_t node_t;

/**
 * struct heap_s - Heap data structure
 *
 * @size: Number of nodes in the heap
 * @data_cmp: Function to compare two node data
 * @root: Pointer to the root node
 */
typedef struct heap_s
{
	size_t size;
	int (*data_cmp)(void *, void *);
	binary_tree_node_t *root;
} heap_t;

/* Heap core API */
heap_t *heap_create(int (*data_cmp)(void *, void *));
binary_tree_node_t *binary_tree_node(binary_tree_node_t *parent, void *data);
binary_tree_node_t *heap_insert(heap_t *heap, void *data);
void *heap_extract(heap_t *heap);
void heap_delete(heap_t *heap, void (*free_data)(void *));
void free_binary_tree_node(node_t *node, void (*free_data)(void *));

/* Debug / display helpers */
void binary_tree_print(const binary_tree_node_t *root,
		       int (*print_data)(char *, void *));

/**
 * SWAP - Swap two variables
 * @a: First variable
 * @b: Second variable
 */
#define SWAP(a, b)              \
	do {                    \
		typeof(a) tmp = (a); \
		(a) = (b);           \
		(b) = tmp;           \
	} while (0)

/**
 * DATASWAP - Swap data fields of two nodes
 * @a: First node
 * @b: Second node
 */
#define DATASWAP(a, b)             \
	do {                       \
		void *tmp = (a)->data; \
		(a)->data = (b)->data; \
		(b)->data = tmp;       \
	} while (0)

#endif /* HEAP_H */