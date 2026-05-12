#include "heap.h"
#include <stdio.h>
#include <stdlib.h>

MinHeap *heap_create(int capacity)
{
    MinHeap *h = malloc(sizeof(MinHeap));
    if (!h) {
        perror("heap_create");
        exit(1);
    }
    h->size = 0;
    h->capacity = capacity;
    h->data = malloc(sizeof(HeapNode) * capacity);
    if (!h->data) {
        perror("heap_create data");
        exit(1);
    }
    return h;
}

void heap_free(MinHeap *h)
{
    free(h->data);
    free(h);
}

int heap_empty(const MinHeap *h)
{
    return h->size == 0;
}

static void heap_swap(HeapNode *a, HeapNode *b)
{
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

static void sift_up(MinHeap *h, int i)
{
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->data[parent].distance > h->data[i].distance) {
            heap_swap(&h->data[parent], &h->data[i]);
            i = parent;
        } else {
            break;
        }
    }
}

static void sift_down(MinHeap *h, int i)
{
    while (1) {
        int smallest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < h->size && h->data[left].distance < h->data[smallest].distance) {
            smallest = left;
        }
        if (right < h->size && h->data[right].distance < h->data[smallest].distance) {
            smallest = right;
        }
        if (smallest == i) break;
        heap_swap(&h->data[i], &h->data[smallest]);
        i = smallest;
    }
}

void heap_push(MinHeap *h, int vertex, int dist)
{
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->data = realloc(h->data, sizeof(HeapNode) * h->capacity);
        if (!h->data) {
            perror("heap_push realloc");
            exit(1);
        }
    }
    h->data[h->size].vertex = vertex;
    h->data[h->size].distance = dist;
    sift_up(h, h->size++);
}

HeapNode heap_pop(MinHeap *h)
{
    HeapNode top = h->data[0];
    h->data[0] = h->data[--h->size];
    if (h->size > 0) {
        sift_down(h, 0);
    }
    return top;
}
