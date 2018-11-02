#include "graph.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int create_vertex (double x, double y, char *label) {
    int id = 0;
    while(graph[id]) id++;
    graph[id] = (vertex*) malloc (sizeof(vertex));
    graph[id] -> id = id;
    graph[id] -> x = x;
    graph[id] -> y = y;
    graph[id] -> on_route = 0;
    sprintf(graph[id] -> label, "%s", label);
    return id;
}

void create_vertex_with_id (int id, double x, double y, char *label) {
    graph[id] = (vertex*) malloc (sizeof(vertex));
    graph[id] -> id = id;
    graph[id] -> x = x;
    graph[id] -> y = y;
    graph[id] -> on_route = 0;
    sprintf(graph[id] -> label, "%s", label);
}

void move_vertex (int id, double x, double y) {
    graph[id] -> x = x;
    graph[id] -> y = y;
}

void remove_vertex (int id) {
    list *edge = edges;
    list *prev = NULL;
    while(edge) {
        if(edge -> a != id && edge -> b != id) {
            prev = edge;
            edge = edge -> tail;
            continue;
        }
        list *next = edge -> tail;
        if(prev) {
            prev -> tail = next;
        }
        else {
            edges = next;
        }
        free(edge);
        edge = next;
    }
    free(graph[id]);
    graph[id] = NULL;
}

int find_vertex_with_label (const char *label) {
    for(int i = 0; i < MAX_VERTICES; i++) {
        if(graph[i] == NULL) continue;
        if(strcmp(label, graph[i] -> label) == 0) {
            return i;
        }
    }
    return -1;
}

void remove_edge (list *edge) {
    list *curr = edges;
    list *prev = NULL;
    while(curr) {
        list *next = curr -> tail;
        if(curr == edge) {
            if(prev) {
                prev -> tail = next;
            }
            else {
                edges = next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = next;
    }
}

void add_edge (int a, int b, int c) {
    if(a == b) return;
    list *edge = edges;
    while(edge) {
        if(edge -> a == a && edge -> b == b) return;
        edge = edge -> tail;
    }
    push_front(&edges, a, b, c);
}

static int dist[MAX_VERTICES];
static int pred[MAX_VERTICES];

int calculate_route (int start, int end) {
    clear_route();
    int vertices_size = 0;
    for(int i = 0; i < MAX_VERTICES; i++) {
        if(graph[i] == NULL) continue;
        vertices_size++;
        dist[i] = INF;
        pred[i] = -1;
    }
    dist[start] = 0;
    while(vertices_size-- > 1) {
        list *edge = edges;
        while(edge != NULL) {
            int u = edge -> a;
            int v = edge -> b;
            int c = edge -> c;
            if(dist[u] + c < dist[v]) {
                dist[v] = dist[u] + c;
                pred[v] = u;
            }
            edge = edge -> tail;
        }
    }
    if(dist[end] == INF) return 0;
    int v = end;
    while(v != -1) {
        graph[v] -> on_route = 1;
        v = pred[v];
    }
    list *edge = edges;
    while(edge != NULL) {
        int u = edge -> a;
        int v = edge -> b;
        int c = edge -> c;
        if(dist[u] + c < dist[v]) {
            return 0;
        }
        if(pred[v] == u && graph[v] -> on_route == 1 && graph[u] -> on_route == 1) {
            edge -> on_route = 1;
        }
        edge = edge -> tail;
    }
    return 1;
}

void clear_route () {
    list *edge = edges;
    while(edge != NULL) {
        edge -> on_route = 0;
        edge = edge -> tail;
    }
    for(int i = 0; i < MAX_VERTICES; i++) {
        if(graph[i] != NULL) graph[i] -> on_route = 0;
    }
}
