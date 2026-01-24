#include "graphs.h"
#include <stdio.h>

void graph_display(const graph_t *graph)
{
    if (graph == NULL) {
        printf("(null)\n");
        return;
    }
    printf("Graph display\n");
}