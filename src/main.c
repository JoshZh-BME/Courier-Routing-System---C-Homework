#include "delivery.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void clear_input(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static int read_int(const char *prompt)
{
    int value;
    printf("%s", prompt);
    while (scanf("%d", &value) != 1) {
        clear_input();
        printf("%s", prompt);
    }
    clear_input();
    return value;
}

static void read_str(const char *prompt, char *buffer, int size)
{
    printf("%s", prompt);
    if (fgets(buffer, size, stdin)) {
        buffer[strcspn(buffer, "\r\n")] = '\0';
    }
}

static void print_locations(const Graph *g)
{
    printf("\n Helyszinek:\n");
    if (g->locations == 0) {
        printf(" [Nincsenek helyszinek]\n");
        return;
    }
    for (int i = 0; i < g->locations; i++) {
        printf(" [%2d] %s\n", i, g->name[i]);
    }
}

int save_graph(const Graph *g, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("save_graph");
        return -1;
    }
    fprintf(f, "Helyszinek szama:%d\n", g->locations);
    for (int i = 0; i < g->locations; i++) {
        fprintf(f, " %d %s\n", i, g->name[i]);
    }
    fprintf(f, "Elek:\n");
    for (int i = 0; i < g->locations; i++) {
        for (AdjNode *curr = g->adj[i]; curr; curr = curr->next) {
            if (curr->dest_vertex > i) {
                fprintf(f, " %d %d %d\n", i, curr->dest_vertex, curr->distance_weight);
            }
        }
    }
    fprintf(f, "Vege\n");
    fclose(f);
    printf(" [Sikeres mentes: %s]\n", filename);
    return 0;
}

int load_graph(Graph *g, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("load_graph");
        return -1;
    }
    for (int i = 0; i < g->locations; i++) {
        AdjNode *curr = g->adj[i];
        while (curr) {
            AdjNode *temp = curr->next;
            free(curr);
            curr = temp;
        }
        g->adj[i] = NULL;
    }
    g->locations = 0;

    char line[256];
    int state = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;

        if (strncmp(line, "Helyszinek szama:", 16) == 0) {
            state = 1;
        } else if (strcmp(line, "Elek:") == 0) {
            state = 2;
        } else if (strcmp(line, "Vege") == 0) {
            break;
        } else if (state == 1) {
            int id;
            char name[MAX_NAME_LEN];
            if (sscanf(line, "%d %63s", &id, name) == 2)
                graph_add_location(g, name);
        } else if (state == 2) {
            int src, dest, w;
            if (sscanf(line, "%d %d %d", &src, &dest, &w) == 3)
                graph_add_edge(g, src, dest, w);
        }
    }
    fclose(f);
    printf(" [Sikeres betoltes: %s]\n", filename);
    return 0;
}

static void menu_graph(Graph *g)
{
    int choice;
    do {
        printf("\n---Terkepe kezelese---\n");
        printf("    1) Helyszin hozzaadasa\n");
        printf("    2) Helyszin eltavolitasa\n");
        printf("    3) Utvonal hozzaadasa\n");
        printf("    4) Utvonal eltavolitasa\n");
        printf("    5) Helyszinek listazasa\n");
        printf("    0) Vissza\n");
        choice = read_int("Valassz egy opciot: ");

        switch (choice) {
        case 1: {
            char name[MAX_NAME_LEN];
            read_str("Add meg a helyszin nevet: ", name, sizeof(name));
            int id = graph_add_location(g, name);
            if (id >= 0)
                printf(" [Helyszin hozzaadva: %s (ID: %d)]\n", name, id);
            else
                printf(" [Hiba: Nem lehet tobb helyszint hozzaadni]\n");
            break;
        }
        case 2: {
            print_locations(g);
            int id = read_int("Add meg a helyszin azonositojat: ");
            if (graph_remove_location(g, id) == 0)
                printf(" [Helyszin eltavolitva: ID %d]\n", id);
            else
                printf(" [Hiba: Nem talalhato helyszin ezzel az azonositoval]\n");
            break;
        }
        case 3: {
            print_locations(g);
            int src = read_int("Add meg a forras helyszin azonositojat: ");
            int dest = read_int("Add meg a cel helyszin azonositojat: ");
            int w = read_int("Add meg a tavolsagot: ");
            graph_add_edge(g, src, dest, w);
            printf(" [Utvonal hozzaadva]\n");
            break;
        }
        case 4: {
            print_locations(g);
            int src = read_int("Add meg a forras helyszin azonositojat: ");
            int dest = read_int("Add meg a cel helyszin azonositojat: ");
            graph_remove_edge(g, src, dest);
            printf(" [Utvonal eltavolitva]\n");
            break;
        }
        case 5:
            graph_print(g);
            break;
        case 0:
            break;
        default:
            printf("[!] Ismeretlen opcio.\n");
        }
    } while (choice != 0);
}

static void menu_routing(const Graph *g)
{
    int choice;
    do {
        printf("\n---Utvonalterv---\n");
        printf("    1) Legrovidebb ut keresese\n");
        printf("    2) Osszes ut keresese\n");
        printf("    0) Vissza\n");
        choice = read_int("Valassz egy opciot: ");

        if (choice == 1) {
            print_locations(g);
            int src = read_int("Add meg a kiindulasi helyszin azonositojat: ");
            int dest = read_int("Add meg a cel helyszin azonositojat: ");
            DijkstraResult res = dijkstra(g, src);
            print_path(&res, g, src, dest);
        } else if (choice == 2) {
            print_locations(g);
            int src = read_int("Add meg a kiindulasi helyszin azonositojat: ");
            DijkstraResult res = dijkstra(g, src);
            printf("\nLegrovidebb utvonalak '%s' helyrol:\n", g->name[src]);
            for (int i = 0; i < g->locations; i++) {
                if (i == src) continue;
                printf("  -> %-20s : ", g->name[i]);
                if (res.dist[i] == INF)
                    printf("Nincs ut\n");
                else
                    printf("Tavolsag: %d km\n", res.dist[i]);
            }
        } else if (choice != 0) {
            printf("[!] Ismeretlen opcio.\n");
        }
    } while (choice != 0);
}

static void menu_delivery(Graph *g, DeliveryQueue *delivery_queue)
{
    int choice;
    do {
        printf("\n---Szallitasok---\n");
        printf("    1) Szallitasi cim hozzaadasa\n");
        printf("    2) Szallitasi cim eltavolitasa\n");
        printf("    3) Szallitasi cimek listazasa\n");
        printf("    4) Szallitasi utvonalterv\n");
        printf("    0) Vissza\n");
        choice = read_int("Valassz egy opciot: ");

        switch (choice) {
        case 1: {
            print_locations(g);
            int loc_id = read_int("Add meg a szallitasi helyszin azonositojat: ");
            int priority = read_int("Add meg a szallitasi prioritast (alacsonyabb ertek magasabb prioritas): ");
            char label[MAX_NAME_LEN];
            read_str("Add meg a szallitasi vevo megnevezeset: ", label, sizeof(label));
            delivery_queue_add(delivery_queue, loc_id, priority, label);
            printf(" [Szallitasi cim hozzaadva]\n");
            break;
        }
        case 2: {
            delivery_queue_print(delivery_queue, g);
            int id = read_int("Add meg a szallitasi cim azonositojat: ");
            delivery_queue_remove(delivery_queue, id);
            printf(" [Szallitasi cim eltavolitva]\n");
            break;
        }
        case 3:
            delivery_queue_print(delivery_queue, g);
            break;
        case 4: {
            print_locations(g);
            int start_loc = read_int("Add meg a kiindulasi helyszin azonositojat: ");
            delivery_queue_route(g, delivery_queue, start_loc);
            break;
        }
        case 0:
            break;
        default:
            printf("[!] Ismeretlen opcio.\n");
        }
    } while (choice != 0);
}

static void menu_file_io(Graph *g)
{
    int choice;
    char filename[256];
    do {
        printf("\n---Fajl I/O ---\n");
        printf("    1) Terkepe mentese\n");
        printf("    2) Terkepe betoltese\n");
        printf("    0) Vissza\n");
        choice = read_int("Valassz egy opciot: ");

        switch (choice) {
        case 1:
            read_str("Add meg a fajl nevet a menteshez: ", filename, sizeof(filename));
            save_graph(g, filename);
            break;
        case 2:
            read_str("Add meg a fajl nevet a betolteshez: ", filename, sizeof(filename));
            load_graph(g, filename);
            break;
        case 0:
            break;
        default:
            printf("[!] Ismeretlen opcio.\n");
        }
    } while (choice != 0);
}

void run_menu(void)
{
    Graph *g = graph_create();
    DeliveryQueue *delivery_queue = delivery_queue_create();

    printf("+------------------------------------+\n");
    printf("|                                    |\n");
    printf("|    Utvonaltervezo rendszer         |\n");
    printf("|    Kotelezo nagy hazi feladat | C  |\n");
    printf("|                                    |\n");
    printf("+------------------------------------+\n");

    int choice;
    do {
        printf("\n+==== Fo menu ====+\n");
        printf("| 1) Terkepe       |\n");
        printf("| 2) Utvonalak     |\n");
        printf("| 3) Szallitasok   |\n");
        printf("| 4) Fajl I/O      |\n");
        printf("| 0) Kilepes       |\n");
        printf("+==================+\n");
        choice = read_int("Valassz egy opciot: ");

        switch (choice) {
        case 1: menu_graph(g);             break;
        case 2: menu_routing(g);           break;
        case 3: menu_delivery(g, delivery_queue); break;
        case 4: menu_file_io(g);           break;
        case 0: printf("\n Kilepes folyamatban...\n\n"); break;
        default: printf("[!] Ismeretlen opcio.\n");
        }
    } while (choice != 0);

    graph_free(g);
    delivery_queue_free(delivery_queue);
}

int main(void)
{
    run_menu();
    return 0;
}
