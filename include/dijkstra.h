#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"
#include "heap.h"

typedef struct {
    int dist[MAX_LOCATION];
    int prev[MAX_LOCATION];
} DijkstraResult;
DijkstraResult dijkstra(const Graph *g, int src);
DijkstraResult dijkstra_array(const Graph *g, int src);
void print_path(const DijkstraResult *r, const Graph *g, int src, int dest);

#endif
