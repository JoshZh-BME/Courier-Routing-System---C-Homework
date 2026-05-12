#include "delivery.h"
#include <stdlib.h>
#include <string.h>

DeliveryQueue *delivery_queue_create(void)
{
    DeliveryQueue *q = calloc(1, sizeof(DeliveryQueue));
    if (!q) {
        perror("delivery_queue_create");
        exit(1);
    }
    return q;
}

void delivery_queue_free(DeliveryQueue *q)
{
    free(q);
}

void delivery_queue_add(DeliveryQueue *q, int location_id, int priority, const char *label)
{
    if (q->count >= MAX_STOPS) {
        printf(" [!] Nem lehet tobb szallitasi cimet hozzaadni, a lista megtelt.\n");
        return;
    }
    DeliveryStop *s = &q->stops[q->count++];
    s->location_id = location_id;
    s->priority = priority;
    strncpy(s->label, label, MAX_NAME_LEN - 1);
    s->label[MAX_NAME_LEN - 1] = '\0';
    delivery_queue_sort(q);
}

void delivery_queue_remove(DeliveryQueue *q, int id)
{
    if (id < 0 || id >= q->count) {
        printf(" [!] Nincs ilyen ID-ju szallitasi cim a listaban.\n");
        return;
    }
    for (int i = id; i < q->count - 1; i++) {
        q->stops[i] = q->stops[i + 1];
    }
    q->count--;
}

void delivery_queue_sort(DeliveryQueue *q)
{
    for (int i = 1; i < q->count; i++) {
        DeliveryStop key = q->stops[i];
        int j = i - 1;
        while (j >= 0 && q->stops[j].priority > key.priority) {
            q->stops[j + 1] = q->stops[j];
            j--;
        }
        q->stops[j + 1] = key;
    }
}

void delivery_queue_print(const DeliveryQueue *q, const Graph *g)
{
    printf("\n=== Szallitasok ===\n");
    if (q->count == 0) {
        printf(" Nincs szallitasi cim a listaban.\n");
        return;
    }
    printf("  %-4s %-8s %-20s %-20s\n", "ID", "Prior.", "Cim", "Helyszin");
    printf("  %-4s %-8s %-20s %-20s\n", "---", "------", "----", "---------");
    for (int i = 0; i < q->count; i++) {
        const DeliveryStop *s = &q->stops[i];
        const char *location = (s->location_id >= 0 && s->location_id < g->locations)
            ? g->name[s->location_id] : "Ismeretlen";
        printf("  %-4d %-8d %-20s %-20s\n", i, s->priority, location, s->label);
    }
    printf("\n");
}

static void print_route_path(const DijkstraResult *r, const Graph *g, int dest)
{
    int path[MAX_LOCATION];
    int path_len = 0;
    int curr = dest;
    while (curr != -1) {
        path[path_len++] = curr;
        curr = r->prev[curr];
    }
    for (int j = path_len - 1; j >= 0; j--) {
        printf("%s", g->name[path[j]]);
        if (j > 0) printf(" -> ");
    }
}

void delivery_queue_route(const Graph *g, const DeliveryQueue *q, int start_location_id)
{
    if (q->count == 0) {
        printf(" Nincs szallitasi cim a listaban.\n");
        return;
    }

    printf("\n+--------------------------------+\n");
    printf("|         Utvonalterv            |\n");
    printf("+--------------------------------+\n");
    printf("Indulasi hely: %s\n\n", g->name[start_location_id]);

    int current_location = start_location_id;
    int total_distance = 0;

    for (int i = 0; i < q->count; i++) {
        const DeliveryStop *s = &q->stops[i];
        int dest = s->location_id;
        printf(" Szallitas %d: [Prioritas %d] %s  (%s)\n",
               i + 1, s->priority, s->label, g->name[dest]);

        DijkstraResult res = dijkstra(g, current_location);
        if (res.dist[dest] == INF) {
            printf("  [!] Nem vezet ut a jelenlegi helyrol a celhoz.\n");
            continue;
        }

        printf("  Utvonal: ");
        print_route_path(&res, g, dest);
        printf("\n  Tavolsag: %d\n\n", res.dist[dest]);

        total_distance += res.dist[dest];
        current_location = dest;
    }

    printf(" Visszateres %s helyre:\n", g->name[start_location_id]);
    DijkstraResult res = dijkstra(g, current_location);
    if (res.dist[start_location_id] != INF) {
        printf("  Utvonal: ");
        print_route_path(&res, g, start_location_id);
        printf("\n  Tavolsag: %d\n", res.dist[start_location_id]);
        total_distance += res.dist[start_location_id];
    } else {
        printf("  [!] Nem vezet ut a jelenlegi helyrol a kiindulasi helyre.\n");
    }

    printf("  ---------------------------------\n");
    printf("  Osszesen: %d km\n", total_distance);
    printf("  Osszesen: %d szallitas\n\n", q->count);
}
