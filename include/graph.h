#ifndef GRAPH_H
#define GRAPH_H

#include "macro.h"
#include <stdio.h>

typedef struct AdjNode {
    int dest_vertex;
    int distance_weight;
    struct AdjNode *next;
} AdjNode;

typedef struct {
    int locations;
    char name[MAX_LOCATION][MAX_NAME_LEN];
    AdjNode *adj[MAX_LOCATION];
} Graph;
Graph *graph_create();
void graph_free(Graph *g);
int graph_add_location(Graph *g, const char *name);
int graph_remove_location(Graph *g, int id);
void graph_add_edge(Graph *g, int src, int dest, int weight);
void graph_remove_edge(Graph *g, int src, int dest);
int graph_find(Graph *g, const char *name);
void graph_print(const Graph *g);

#endif
