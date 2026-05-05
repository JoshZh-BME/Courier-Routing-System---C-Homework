#include "delivery.h"
#include <stdlib.h>
#include <string.h>

static void clear_input()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

static int read_int(const char *prompt)
{
    int value;
    printf("%s", prompt);
    while (scanf("%d", &value) != 1)
    {
        clear_input();
        printf("%s", prompt);
    }
    clear_input();
    return value;
}

static void read_str(const char *prompt, char *buffer, int size)
{
    printf("%s", prompt);
    if (fgets(buffer, size, stdin))
    {
        buffer[strcspn(buffer, "\r\n")] = '\0';
    }
}

static void print_locations(const Graph *g)
{
    printf("\n Helyszínek:\n");
    if (g->locations == 0)
    {
        printf(" [Nincsenek helyszínek]\n");
        return;
    }
    for (int i = 0; i < g->locations; i++)
    {
        printf(" [%2d] %s\n", i, g->name[i]);
    }
}

int save_graph(const Graph *g, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f)
    {
        perror("save_graph");
        return -1;
    }
    fprintf(f, "Helyszínök száma:%d\n", g->locations);
    for (int i = 0; i < g->locations; i++)
    {
        fprintf(f, " %d %s\n", i, g->name[i]);
    }
    fprintf(f, "Élek:\n");
    for (int i = 0; i < g->locations; i++)
    {
        for (AdjNode *curr = g->adj[i]; curr; curr = curr->next)
        {
            if (curr->distance_weight > i)
            {
                fprintf(f, " %d %d %d\n", i, curr->dest_vertex, curr->distance_weight);
            }
        }
    }
    fprintf(f, "Vége\n");
    fclose(f);
    printf(" [Sikeres mentés: %s]\n", filename);
    return 0;
}

int load_graph(Graph *g, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        perror("load_graph");
        return -1;
    }
    for (int i = 0; i < g->locations; i++)
    {
        AdjNode *curr = g->adj[i];
        while (curr)
        {
            AdjNode *temp = curr->next;
            free(curr);
            curr = temp;
        }
        g->adj[i] = NULL;
    }
    g->locations = 0;
    char line[256];
    int state = 0;
    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
            continue;
        if (strncmp(line, "Helyszínök száma:", 9) == 0)
        {
            state = 1;
        }
        else if (strcmp(line, "Élek:") == 0)
        {
            state = 2;
        }
        else if (strcmp(line, "Vége") == 0)
        {
            break;
        }
        else if (state == 1)
        {
            int id;
            char name[MAX_NAME_LEN];
            if (sscanf(line, "%d %63s", &id, name) == 2)
                graph_add_location(g, name);
        }
        else if (state == 2)
        {
            int src, dest, w;
            if (sscanf(line, "%d %d %d", &src, &dest, &w) == 3)
                graph_add_edge(g, src, dest, w);
        }
    }
    fclose(f);
    printf(" [Sikeres betöltés: %s]\n", filename);
    return 0;
}

static void menu_graph(Graph *g)
{
    int choice;
    do
    {
        printf("\n---Térkép kezelése---\n");
        printf("    1) Helyszín hozzáadása\n");
        printf("    2) Helyszín eltávolítása\n");
        printf("    3) Útvonal hozzáadása\n");
        printf("    4) Útvonal eltávolítása\n");
        printf("    5) Helyszínek listázása\n");
        printf("    0) Vissza\n");
        choice = read_int("Válassz egy opciót: ");
        switch (choice)
        {
        case 1:
        {
            char name[MAX_NAME_LEN];
            read_str("Add meg a helyszín nevét: ", name, sizeof(name));
            int id = graph_add_location(g, name);
            if (id >= 0)
                printf(" [Helyszín hozzáadva: %s (ID: %d)]\n", name, id);
            else
                printf(" [Hiba: Nem lehet több helyszínt hozzáadni]\n");
            break;
        }
        case 2:
        {
            print_locations(g);
            int id = read_int("Add meg a helyszín azonosítóját: ");
            if (graph_remove_location(g, id) == 0)
            {
                printf(" [Helyszín eltávolítva: ID %d]\n", id);
            }
            else
            {
                printf(" [Hiba: Nem található helyszín ezzel az azonosítóval]\n");
            }
            break;
        }
        case 3:
        {
            print_locations(g);
            int src = read_int("Add meg a forrás helyszín azonosítóját: ");
            int dest = read_int("Add meg a cél helyszín azonosítóját: ");
            int w = read_int("Add meg a távolságot: ");
            graph_add_edge(g, src, dest, w);
            printf(" [Útvonal hozzáadva]\n");
            break;
        }
        case 4:
        {
            print_locations(g);
            int src = read_int("Add meg a forrás helyszín azonosítóját: ");
            int dest = read_int("Add meg a cél helyszín azonosítóját: ");
            graph_remove_edge(g, src, dest);
            printf(" [Útvonal eltávolítva]\n");
            break;
        }
        case 5:
            graph_print(g);
            break;
        case 0:
            break;
        default:
            printf("[!] Ismeretlen opció.\n");
        }
    } while (choice != 0);
}

static void menu_routing(const Graph *g)
{
    int choice;
    do
    {
        printf("\n---Útvonalterv---\n");
        printf("    1) Legrövidebb út keresése\n");
        printf("    2) Összes út keresése\n");
        printf("    0) Vissza\n");
        choice = read_int("Válassz egy opciót: ");
        if (choice == 1)
        {
            print_locations(g);
            int src = read_int("Add meg a kiindulási helyszín azonosítóját: ");
            int dest = read_int("Add meg a cél helyszín azonosítóját: ");
            DijkstraResult res = dijkstra(g, src);
            print_path(&res, g, src, dest);
        }
        else if (choice == 2)
        {
            print_locations(g);
            int src = read_int("Add meg a kiindulási helyszín azonosítóját: ");
            DijkstraResult res = dijkstra(g, src);
            printf("\nLegrövidebb útvonalak '%s' helyről:\n", g->name[src]);
            for (int i = 0; i < g->locations; i++)
            {
                if (i == src)
                    continue;
                printf("  -> %-20s : ", g->name[i]);
                if (res.dist[i] == INF)
                {
                    printf("Nincs út\n");
                }
                else
                {
                    printf("Távolság: %d km\n", res.dist[i]);
                }
            }
        }
        else if (choice == 0)
        {
            break;
        }
        else
        {
            printf("[!] Ismeretlen opció.\n");
        }
    } while (choice != 0);
}

static void menu_delivery(Graph *g, DeliveryQueue *delivery_queue)
{
    int choice;
    do
    {
        printf("\n---Szállítások---\n");
        printf("    1) Szállítási cím hozzáadása\n");
        printf("    2) Szállítási cím eltávolítása\n");
        printf("    3) Szállítási címek listázása\n");
        printf("    4) Szállítási útvonalterv\n");
        printf("    0) Vissza\n");
        choice = read_int("Válassz egy opciót: ");
        switch (choice)
        {
        case 1:
        {
            print_locations(g);
            int loc_id = read_int("Add meg a szállítási helyszín azonosítóját: ");
            int priority = read_int("Add meg a szállítási prioritást (alacsonyabb érték magasabb prioritás): ");
            char label[MAX_NAME_LEN];
            read_str("Add meg a szállítási cím megnevezését: ", label, sizeof(label));
            delivery_queue_add(delivery_queue, loc_id, priority, label);
            printf(" [Szállítási cím hozzáadva]\n");
            break;
        }
        case 2:
        {
            delivery_queue_print(delivery_queue, g);
            int id = read_int("Add meg a szállítási cím azonosítóját: ");
            delivery_queue_remove(delivery_queue, id);
            printf(" [Szállítási cím eltávolítva]\n");
            break;
        }
        case 3:
            delivery_queue_print(delivery_queue, g);
            break;
        case 4:
        {
            print_locations(g);
            int start_loc = read_int("Add meg a kiindulási helyszín azonosítóját: ");
            delivery_queue_route(g, delivery_queue, start_loc);
            break;
        }
        case 0:
            break;
        default:
            printf("[!] Ismeretlen opció.\n");
        }
    } while (choice != 0);
}

static void menu_file_io(Graph *g)
{
    int choice;
    char filename[256];
    do
    {
        printf("\n---Fájl I/O ---\n");
        printf("    1) Térkép mentése  \n");
        printf("    2) Térkép betöltése\n");
        printf("    0) Vissza\n");
        choice = read_int("Válassz egy opciót: ");
        switch (choice)
        {
        case 1:
            read_str("Add meg a fájl nevét a mentéshez: ", filename, sizeof(filename));
            save_graph(g, filename);
            break;
        case 2:
            read_str("Add meg a fájl nevét a betöltéshez: ", filename, sizeof(filename));
            load_graph(g, filename);
            break;
        case 0:
            break;
        default:
            printf("[!] Ismeretlen opció.\n");
        }
    } while (choice != 0);
}

void run_menu()
{
    Graph *g = graph_create();
    DeliveryQueue *delivery_queue = delivery_queue_create();

    printf("╔══════════════════════════════════════╗\n");
    printf("║                                      ║\n");
    printf("║    Útvonaltervező rendszer           ║\n");
    printf("║    Kötelező nagy házi feladat | C    ║\n");
    printf("║                                      ║\n");
    printf("╚══════════════════════════════════════╝\n");

    int choice;
    do
    {
        printf("\n╔═══ Fő menü ════╗\n");
        printf("║ 1) Térkép        ║\n");
        printf("║ 2) Útvonalak     ║\n");
        printf("║ 3) Szállítások   ║\n");
        printf("║ 4) Fájl I/O      ║\n");
        printf("║ 0) Kilépés       ║\n");
        printf("╚══════════════════╝\n");
        choice = read_int("Válassz egy opciót: ");

        switch (choice)
        {
        case 1:
            menu_graph(g);
            break;
        case 2:
            menu_routing(g);
            break;
        case 3:
            menu_delivery(g, delivery_queue);
            break;
        case 4:
            menu_file_io(g);
            break;
        case 0:
            printf("\n Kilépés folyamatban...\n\n");
            break;
        default:
            printf("[!] Ismeretlen opció.\n");
        }
    } while (choice != 0);

    graph_free(g);
    delivery_queue_free(delivery_queue);
}

int main()
{
    run_menu();
    return 0;
}
