#ifndef INTERFACE
#define INTERFACE

#include <gtk/gtk.h>
#include "graph.h"
#include "list.h"
#include "saving.h"

#define H 700
#define W 1100
#define MAP_W 950

#define VERTEX_RADIUS 25
#define FONT_SIZE 14
#define LINE_WIDTH 1
#define PATH_WIDTH 2
#define VECTOR_LENGTH 20
#define ARROW_SIZE 13

#define LINE_COLOR 0.45
#define TEXT_COLOR 0.3
#define VERTEX_COLOR 1
#define RED_COLOR 1
#define BACKGROUND_COLOR 0.9

GtkWidget* generate_window ();

#endif
