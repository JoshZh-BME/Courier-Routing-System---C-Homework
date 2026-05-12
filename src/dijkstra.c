#include "dijkstra.h"
#include <stdlib.h>

DijkstraResult dijkstra(const Graph *g, int src)
{
    DijkstraResult r;
    int n = g->locations;

    for (int i = 0; i < n; i++) {
        r.dist[i] = INF;
        r.prev[i] = -1;
    }
    r.dist[src] = 0;

    MinHeap *pq = heap_create(n * 2);
    heap_push(pq, src, 0);

    while (!heap_empty(pq)) {
        HeapNode curr = heap_pop(pq);
        int u = curr.vertex;
        if (curr.distance > r.dist[u]) continue;

        for (AdjNode *adj = g->adj[u]; adj; adj = adj->next) {
            int v = adj->dest_vertex;
            int new_dist = r.dist[u] + adj->distance_weight;
            if (new_dist < r.dist[v]) {
                r.dist[v] = new_dist;
                r.prev[v] = u;
                heap_push(pq, v, new_dist);
            }
        }
    }

    heap_free(pq);
    return r;
}

static void print_path_recursive(const DijkstraResult *r, const Graph *g, int dest)
{
    if (r->prev[dest] == -1) {
        printf("%s", g->name[dest]);
        return;
    }
    print_path_recursive(r, g, r->prev[dest]);
    printf(" -> %s", g->name[dest]);
}

void print_path(const DijkstraResult *r, const Graph *g, int src, int dest)
{
    if (r->dist[dest] == INF) {
        printf(" Nincs ut a %s helyrol a %s helyre.\n", g->name[src], g->name[dest]);
        return;
    }
    printf(" Utvonal: ");
    print_path_recursive(r, g, dest);
    printf("\n Tavolsag: %d\n", r->dist[dest]);
}
