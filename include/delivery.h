#ifndef DELIVERY_H
#define DELIVERY_H

#include "dijkstra.h"

typedef struct {
    int location_id;
    int priority;
    char label[MAX_NAME_LEN];
} DeliveryStop;

typedef struct {
    DeliveryStop stops[MAX_STOPS];
    int count;
} DeliveryQueue;
DeliveryQueue *delivery_queue_create();
void delivery_queue_free(DeliveryQueue *q);
void delivery_queue_add(DeliveryQueue *q, int location_id, int priority, const char *label);
void delivery_queue_remove(DeliveryQueue *q, int id);
void delivery_queue_sort(DeliveryQueue *q);
void delivery_queue_print(const DeliveryQueue *q, const Graph *g);
void delivery_queue_route(const Graph *g, const DeliveryQueue *q, int start_location_id);

#endif
