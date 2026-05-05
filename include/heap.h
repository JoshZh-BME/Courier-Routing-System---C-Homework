#ifndef HEAP_H
#define HEAP_H

#include "lib.h"

// --- Adatszerkezetek --- //
typedef struct
{
    int vertex;
    int distance;
} HeapNode; // Mininális bináris kupac elemei

typedef struct
{
    int size;
    int capacity;
    HeapNode *data;
} MinHeap; // Minimális bináris kupac, priority queue-hoz

// --- Függvénydeklarációk --- //
MinHeap *heap_create(int capacity);
void heap_free(MinHeap *h);
void heap_push(MinHeap *h, int vertex, int dist);
HeapNode heap_pop(MinHeap *h);
int heap_empty(const MinHeap *h);

#endif
