#include "queues.h"
#include <stdlib.h>

queue_node_t *queue_push_front(queue_t *queue, void *ptr)
{
    queue_node_t *new_node;
    
    if (!queue || !ptr)
        return NULL;
    
    new_node = malloc(sizeof(queue_node_t));
    if (!new_node)
        return NULL;
    
    new_node->ptr = ptr;
    new_node->next = queue->front;
    new_node->prev = NULL;
    
    if (queue->front)
        queue->front->prev = new_node;
    else
        queue->back = new_node;
    
    queue->front = new_node;
    return new_node;
}

queue_node_t *queue_push_back(queue_t *queue, void *ptr)
{
    queue_node_t *new_node;
    
    if (!queue || !ptr)
        return NULL;
    
    new_node = malloc(sizeof(queue_node_t));
    if (!new_node)
        return NULL;
    
    new_node->ptr = ptr;
    new_node->next = NULL;
    new_node->prev = queue->back;
    
    if (queue->back)
        queue->back->next = new_node;
    else
        queue->front = new_node;
    
    queue->back = new_node;
    return new_node;
}