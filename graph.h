#ifndef GRAPH
#define GRAPH

#include "list.h"

#define MAX_VERTICES 100
#define LABEL_LENGTH 10
#define INF 1 << 30

typedef struct vertex {
    int id;
    double x, y;
    int clicked;
    int on_route;
    char label[LABEL_LENGTH];
} vertex;

vertex* graph[MAX_VERTICES];
list *edges;

int create_vertex (double x, double y, char *label);
void create_vertex_with_id (int id, double x, double y, char *label);
void move_vertex (int id, double x, double y);
void remove_vertex (int id);
int find_vertex_with_label (const char *label);
int calculate_route (int start, int end);
void clear_route ();

void add_edge (int a, int b, int c);
void remove_edge (list *edge);

#endif
