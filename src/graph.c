#include "graph.h"

Graph *graph_create(){
    Graph *g = calloc(1, sizeof(Graph));
    if (!g){
        perror("graph_create");
        exit(1);
    }
    return g;
}

void graph_free(Graph *g){
    if (!g) return;
    for (int i = 0; i < g->locations; i++){
        AdjNode *curr = g->adj[i];
        while (curr){
            AdjNode *temp = curr->next;
            free(curr);
            curr = temp;
        }
    }
    free(g);
}

int graph_add_location(Graph *g, const char *name){
    if (g->locations >= MAX_LOCATION) return -1;
    int id = g->locations++;
    strncpy(g->name[id], name, MAX_NAME_LEN - 1);
    g->name[id][MAX_NAME_LEN - 1] = '\0';
    g->adj[id] = NULL;
    return id;
}

int graph_remove_location(Graph *g, int id){
    if (id < 0 || id >= g->locations) return -1;
    AdjNode *curr = g->adj[id];
    while (curr){
        AdjNode *temp = curr->next;
        free(curr);
        curr = temp;
    }
    for (int i = 0; i < g->locations; i++){
        if (i == id) continue;
        AdjNode **pp = &g->adj[i];
        while (*pp){
            if ((*pp)->dest_vertex == id){
                AdjNode *temp = (*pp)->next;
                free(*pp);
                *pp = temp;
            }
            else
            {
                if ((*pp)->dest_vertex > id) (*pp)->dest_vertex--;
                pp = &(*pp)->next;
            }
        }
    }
    for(int i = id; i < g->locations - 1; i++){
        strncpy(g->name[i], g->name[i + 1], MAX_NAME_LEN - 1);
        g->adj[i] = g->adj[i + 1];
    }
    g->locations--;
    return 0;
}

static AdjNode *new_adj_node(int dest, int weight){
    AdjNode *node = malloc(sizeof(AdjNode));
    if (!node){
        perror("new_adj_node");
        exit(1);
    }
    node->dest_vertex = dest;
    node->distance_weight = weight;
    node->next = NULL;
    return node;
}

void graph_add_edge(Graph *g, int src, int dest, int weight){
    AdjNode *node1 = new_adj_node(dest, weight);
    node1->next = g->adj[src];
    g->adj[src] = node1;

    AdjNode *node2 = new_adj_node(src, weight);
    node2->next = g->adj[dest];
    g->adj[dest] = node2;
}

void graph_remove_edge(Graph *g, int src, int dest){
    AdjNode **pp = &g->adj[src];
    while (*pp){
        if ((*pp)->dest_vertex == dest){
            AdjNode *temp = (*pp)->next;
            free(*pp);
            *pp = temp;
            break;
        } else {
            pp = &(*pp)->next;
        }
    }
    pp = &g->adj[dest];
    while (*pp){
        if ((*pp)->dest_vertex == src){
            AdjNode *temp = (*pp)->next;
            free(*pp);
            *pp = temp;
            break;
        } else {
            pp = &(*pp)->next;
        }
    }
}

int graph_find(Graph *g, const char *name){
    for (int i = 0; i < g->locations; i++){
        if (strcmp(g->name[i], name) == 0) return i;
    }
    return -1;
}

void graph_print(const Graph *g){
    printf("\n=== Éllista ===\n");
    for (int i = 0; i < g->locations; i++){
        printf("  [%2d] %-20s ->", i, g->name[i]);
        AdjNode *curr = g->adj[i];
        if(!curr){
            printf(" (no edges)\n");
            continue;
        }
        while (curr){
            printf("%s(%d)", g->name[curr->dest_vertex], curr->distance_weight);
            if (curr->next) printf(" ->");
            curr = curr->next;
        }
        printf("\n");
    }
    printf("\n");
}
