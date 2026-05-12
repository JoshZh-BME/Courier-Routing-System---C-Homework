#ifndef HEAP_H
#define HEAP_H

typedef struct {
    int vertex;
    int distance;
} HeapNode;

typedef struct {
    int size;
    int capacity;
    HeapNode *data;
} MinHeap;
MinHeap *heap_create(int capacity);
void heap_free(MinHeap *h);
void heap_push(MinHeap *h, int vertex, int dist);
HeapNode heap_pop(MinHeap *h);
int heap_empty(const MinHeap *h);

#endif
