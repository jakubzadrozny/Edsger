#include "saving.h"
#include <stdlib.h>
#include <stdio.h>

void load_map_state () {
    while(edges != NULL) {
        remove_edge(edges);
    }
    for(int i = 0; i < MAX_VERTICES; i++) {
        remove_vertex(i);
    }

    FILE *map = fopen("map.txt", "r");

    int id;
    double x, y;
    char buffer[20];

    fscanf(map, "%d", &id);
    while(id != -1) {
        fscanf(map, "%lf %lf %s", &x, &y, buffer);
        create_vertex_with_id(id, x, y, buffer);
        fscanf(map, "%d", &id);
    }

    int a, b, c;
    fscanf(map, "%d", &a);
    while(a != -1) {
        fscanf(map, "%d %d", &b, &c);
        add_edge(a, b, c);
        fscanf(map, "%d", &a);
    }
}

void save_map_state () {
    remove("map.txt");
    FILE *map = fopen("map.txt", "w");

    for(int i = 0; i < MAX_VERTICES; i++) {
        if(graph[i] == NULL) continue;
        fprintf(map, "%d %lf %lf %s\n", i, graph[i] -> x, graph[i] -> y, graph[i] -> label);
    }

    fprintf(map, "-1\n");

    list *edge = edges;
    while(edge != NULL) {
        fprintf(map, "%d %d %d\n", edge -> a, edge -> b, edge -> c);
        edge = edge -> tail;
    }

    fprintf(map, "-1\n");

    fclose(map);
}
