#include "interface.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

static void redraw_map ();

static void add_vertex_clicked (GtkButton *button, gpointer data);
static void find_route_clicked (GtkButton *button, gpointer data);
static void clear_route_clicked (GtkButton *button, gpointer data);
static void destroy_vertex_clicked (GtkMenuItem *item, gpointer data);
static void add_edge_clicked (GtkMenuItem *item, gpointer data);
static void delete_edge_clicked (GtkMenuItem *item, gpointer data);
static void save_map_clicked (GtkMenuItem *item, gpointer data);
static void load_map_clicked (GtkMenuItem *item, gpointer data);

static void open_label_dialog (GtkMenuItem *item, gpointer data);
static void open_weight_dialog (GtkMenuItem *item, gpointer data);

static void weight_dialog_response (GtkDialog *dialog, gint response_id, gpointer data);
static void label_dialog_response (GtkDialog *dialog, gint response_id, gpointer data);

static gboolean draw_map (GtkWidget *widget, cairo_t *cr, gpointer data);
static gboolean map_clicked (GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean map_released (GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean pointer_moved (GtkWidget *widget, GdkEventMotion *event, gpointer data);

static int clicked = -1;
static int right_clicked = -1;
static int new_edge = -1;

static list *edge_clicked = NULL;

static double offset_x = 0;
static double offset_y = 0;
static double mouse_x;
static double mouse_y;

static GtkWidget *window;
static GtkWidget *drawing_area;
static GtkWidget *popup;
static GtkWidget *edge_popup;
static GtkWidget *start_select;
static GtkWidget *destination_select;

static char buffer[20];

GtkWidget* generate_window () {
    GtkWidget *add_vertex;
    GtkWidget *find_route;
    GtkWidget *clear_route;
    GtkWidget *side_box;
    GtkWidget *main_box;
    GtkWidget *map_box;
    GtkWidget *destroy_vertex;
    GtkWidget *add_edge;
    GtkWidget *edit_label;
    GtkWidget *edit_weight;
    GtkWidget *delete_edge;
    GtkWidget *save_map;
    GtkWidget *load_map;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW(window), "Edsger");
    gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(window, W, H);

    add_vertex = gtk_button_new_with_label("Add vertex");
    gtk_widget_set_size_request(add_vertex, -1, 50);
    g_signal_connect (G_OBJECT(add_vertex), "clicked", G_CALLBACK(add_vertex_clicked), NULL);

    find_route = gtk_button_new_with_label("Find route");
    gtk_widget_set_size_request(find_route, -1, 50);
    g_signal_connect (G_OBJECT(find_route), "clicked", G_CALLBACK(find_route_clicked), NULL);

    clear_route = gtk_button_new_with_label("Clear route");
    gtk_widget_set_size_request(clear_route, -1, 50);
    g_signal_connect (G_OBJECT(clear_route), "clicked", G_CALLBACK(clear_route_clicked), NULL);

    save_map = gtk_button_new_with_label("Save map");
    gtk_widget_set_size_request(save_map, -1, 50);
    g_signal_connect (G_OBJECT(save_map), "clicked", G_CALLBACK(save_map_clicked), NULL);

    load_map = gtk_button_new_with_label("Load map");
    gtk_widget_set_size_request(load_map, -1, 50);
    g_signal_connect (G_OBJECT(load_map), "clicked", G_CALLBACK(load_map_clicked), NULL);

    start_select = gtk_combo_box_text_new_with_entry();
    gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(start_select), 0, "Select start");
    gtk_combo_box_set_active(GTK_COMBO_BOX(start_select), 0);
    gtk_widget_set_size_request(start_select, -1, 50);

    destination_select = gtk_combo_box_text_new_with_entry();
    gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(destination_select), 0, "Select destination");
    gtk_combo_box_set_active(GTK_COMBO_BOX(destination_select), 0);
    gtk_widget_set_size_request(destination_select, -1, 50);

    side_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width (GTK_CONTAINER(side_box), 30);
    gtk_box_set_spacing(GTK_BOX(side_box), 30);

    gtk_box_pack_start(GTK_BOX(side_box), start_select, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), destination_select, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), find_route, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), clear_route, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), add_vertex, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), save_map, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), load_map, FALSE, FALSE, 0);

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, MAP_W, H);
    g_signal_connect (G_OBJECT (drawing_area), "draw", G_CALLBACK (draw_map), NULL);
    g_signal_connect (G_OBJECT (drawing_area), "motion-notify-event", G_CALLBACK(pointer_moved), NULL);
    g_signal_connect (G_OBJECT (drawing_area), "button-press-event", G_CALLBACK(map_clicked), NULL);
    g_signal_connect (G_OBJECT (drawing_area), "button-release-event", G_CALLBACK(map_released), NULL);
    gtk_widget_set_events(drawing_area, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);

    map_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(map_box), drawing_area, TRUE, TRUE, 0);

    main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(main_box), side_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), map_box, TRUE, TRUE, 0);

    GdkRGBA bg_color;
    gdk_rgba_parse(&bg_color, "#fff");
    gtk_widget_override_background_color(main_box, GTK_STATE_FLAG_NORMAL, &bg_color);

    gtk_container_add (GTK_CONTAINER(window), main_box);

    popup = gtk_menu_new();

    add_edge = gtk_menu_item_new_with_label("New edge");
    gtk_menu_shell_append(GTK_MENU_SHELL(popup), add_edge);
    g_signal_connect(G_OBJECT(add_edge), "activate", G_CALLBACK(add_edge_clicked), NULL);
    gtk_widget_show(add_edge);

    edit_label = gtk_menu_item_new_with_label("Edit label");
    gtk_menu_shell_append(GTK_MENU_SHELL(popup), edit_label);
    g_signal_connect(G_OBJECT(edit_label), "activate", G_CALLBACK(open_label_dialog), NULL);
    gtk_widget_show(edit_label);

    destroy_vertex = gtk_menu_item_new_with_label("Destroy vertex");
    gtk_menu_shell_append(GTK_MENU_SHELL(popup), destroy_vertex);
    g_signal_connect(G_OBJECT(destroy_vertex), "activate", G_CALLBACK(destroy_vertex_clicked), NULL);
    gtk_widget_show(destroy_vertex);

    edge_popup = gtk_menu_new();

    edit_weight = gtk_menu_item_new_with_label("Edit weight");
    gtk_menu_shell_append(GTK_MENU_SHELL(edge_popup), edit_weight);
    g_signal_connect(G_OBJECT(edit_weight), "activate", G_CALLBACK(open_weight_dialog), NULL);
    gtk_widget_show(edit_weight);

    delete_edge = gtk_menu_item_new_with_label("Remove edge");
    gtk_menu_shell_append(GTK_MENU_SHELL(edge_popup), delete_edge);
    g_signal_connect(G_OBJECT(delete_edge), "activate", G_CALLBACK(delete_edge_clicked), NULL);
    gtk_widget_show(delete_edge);

    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (gtk_main_quit), NULL);

    gtk_window_set_focus(GTK_WINDOW(window), find_route);

    return window;
}

static gboolean draw_map (GtkWidget *widget, cairo_t *cr, gpointer data) {
    cairo_set_source_rgb(cr, BACKGROUND_COLOR, BACKGROUND_COLOR, BACKGROUND_COLOR);
    cairo_paint(cr);

    cairo_text_extents_t extents;

    cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, FONT_SIZE);

    list *edge = edges;
    while(edge != NULL) {
        int a = edge -> a;
        int b = edge -> b;

        if(edge -> on_route) {
            cairo_set_source_rgb (cr, RED_COLOR, 0, 0);
            cairo_set_line_width (cr, PATH_WIDTH);
        }
        else {
            cairo_set_source_rgb (cr, LINE_COLOR, LINE_COLOR, LINE_COLOR);
            cairo_set_line_width (cr, LINE_WIDTH);
        }

        cairo_move_to(cr, graph[a] -> x, graph[a] -> y);
        cairo_line_to(cr, graph[b] -> x, graph[b] -> y);

        double vx = graph[b] -> y - graph[a] -> y;
        double vy = graph[a] -> x - graph[b] -> x;
        double length = sqrt(vx * vx + vy * vy);
        double text_vx = vx * VECTOR_LENGTH / length;
        double text_vy = vy * VECTOR_LENGTH / length;
        double text_x = (graph[a] -> x + graph[b] -> x) / 2 + text_vx;
        double text_y = (graph[a] -> y + graph[b] -> y) / 2 + text_vy;
        sprintf(buffer, "%d", edge -> c);
        cairo_text_extents(cr, buffer, &extents);
        cairo_move_to(cr, text_x - extents.width / 2, text_y + extents.height / 2);
        cairo_show_text(cr, buffer);

        double vector_x = graph[b] -> x - graph[a] -> x;
        double vector_y = graph[b] -> y - graph[a] -> y;
        double vector_length = sqrt(vector_x * vector_x + vector_y * vector_y);
        vector_x *= (vector_length - VERTEX_RADIUS - ARROW_SIZE) / vector_length;
        vector_y *= (vector_length - VERTEX_RADIUS - ARROW_SIZE) / vector_length;
        double px = graph[a] -> x + vector_x;
        double py = graph[a] -> y + vector_y;
        double arrow_vx = vx * (ARROW_SIZE / 2) / length;
        double arrow_vy = vy * (ARROW_SIZE / 2) / length;
        double temp_vx = -2 * arrow_vy;
        double temp_vy = 2 * arrow_vx;
        double p1x = px + arrow_vx;
        double p1y = py + arrow_vy;
        double p2x = px - arrow_vx;
        double p2y = py - arrow_vy;
        double p3x = px + temp_vx;
        double p3y = py + temp_vy;

        cairo_move_to(cr, p1x, p1y);
        cairo_line_to(cr, p2x, p2y);
        cairo_line_to(cr, p3x, p3y);
        cairo_line_to(cr, p1x, p1y);

        cairo_stroke_preserve(cr);
        cairo_fill(cr);

        edge = edge -> tail;
    }
    if(new_edge != -1) {
        cairo_move_to(cr, graph[new_edge] -> x, graph[new_edge] -> y);
        cairo_line_to(cr, mouse_x, mouse_y);
        cairo_set_line_width (cr, LINE_WIDTH);
        cairo_set_source_rgb (cr, LINE_COLOR, LINE_COLOR, LINE_COLOR);
        cairo_stroke_preserve(cr);
        cairo_fill(cr);
    }

    for(int i = 0; i < MAX_VERTICES; i++) {
        if(graph[i] == NULL) continue;

        if(graph[i] -> on_route) {
            cairo_set_source_rgb (cr, RED_COLOR, 0, 0);
            cairo_set_line_width (cr, PATH_WIDTH);
        }
        else {
            cairo_set_source_rgb (cr, LINE_COLOR, LINE_COLOR, LINE_COLOR);
            cairo_set_line_width (cr, LINE_WIDTH);
        }
        cairo_move_to(cr, graph[i] -> x, graph[i] -> y);
        cairo_arc(cr, graph[i] -> x, graph[i] -> y, VERTEX_RADIUS, 0, 2 * M_PI);
        cairo_stroke(cr);

        cairo_arc(cr, graph[i] -> x, graph[i] -> y, VERTEX_RADIUS - 0.5, 0, 2 * M_PI);
        cairo_set_source_rgb (cr, VERTEX_COLOR, VERTEX_COLOR, VERTEX_COLOR);
        cairo_fill(cr);

        if(graph[i] -> on_route) {
            cairo_set_source_rgb (cr, RED_COLOR, 0, 0);
        }
        else {
            cairo_set_source_rgb (cr, TEXT_COLOR, TEXT_COLOR, TEXT_COLOR);
        }
        cairo_text_extents(cr, graph[i] -> label, &extents);
        cairo_move_to(cr, graph[i] -> x - extents.width / 2, graph[i] -> y + extents.height / 2);
        cairo_show_text(cr, graph[i] -> label);
    }

    return FALSE;
}

static gboolean map_clicked (GtkWidget *widget, GdkEventButton *event, gpointer data) {
    double click_x = event -> x;
    double click_y = event -> y;

    int closest = -1;
    double dx, dy;
    for(int i = 0; i < MAX_VERTICES; i++) {
        double dist;
        if(graph[i] == NULL) continue;
        dx = graph[i] -> x - click_x;
        dy = graph[i] -> y - click_y;
        dist = sqrt(dx * dx + dy * dy);
        if(dist < VERTEX_RADIUS) {
            closest = i;
            break;
        }
    }

    list *closest_edge = NULL;
    if(closest == -1) {
        double best_dist = ARROW_SIZE;
        list *edge = edges;
        while(edge != NULL) {
            int a = edge -> a;
            int b = edge -> b;

            double vector_x = graph[b] -> x - graph[a] -> x;
            double vector_y = graph[b] -> y - graph[a] -> y;
            double vector_length = sqrt(vector_x * vector_x + vector_y * vector_y);
            vector_x *= (vector_length - VERTEX_RADIUS - ARROW_SIZE) / vector_length;
            vector_y *= (vector_length - VERTEX_RADIUS - ARROW_SIZE) / vector_length;
            double px = graph[a] -> x + vector_x;
            double py = graph[a] -> y + vector_y;

            double ddx = px - click_x;
            double ddy = py - click_y;
            double dist = sqrt(ddx * ddx + ddy * ddy);

            if(dist < best_dist) {
                closest_edge = edge;
                best_dist = dist;
            }

            edge = edge -> tail;
        }
    }

    if(new_edge != -1) {
        if(closest != -1) {
            add_edge(new_edge, closest, 1);
        }
        new_edge = -1;
        redraw_map();
    }
    else if(event -> button == 1) {
        clicked = closest;
        offset_x = dx;
        offset_y = dy;
    }
    else {
        if(closest != -1) {
            right_clicked = closest;
            gtk_menu_popup(GTK_MENU(popup), NULL, NULL, NULL, NULL, event -> button, gdk_event_get_time((GdkEvent*)event));
        }
        else if(closest_edge != NULL) {
            edge_clicked = closest_edge;
            gtk_menu_popup(GTK_MENU(edge_popup), NULL, NULL, NULL, NULL, event -> button, gdk_event_get_time((GdkEvent*)event));
        }
    }

    return FALSE;
}

static gboolean map_released (GtkWidget *widget, GdkEventButton *event, gpointer data) {
    if(clicked != -1) {
        clicked = -1;
    }
    return FALSE;
}

static gboolean pointer_moved (GtkWidget *widget, GdkEventMotion *event, gpointer data) {
    double x = event -> x;
    double y = event -> y;
    if(clicked != -1) {
        move_vertex(clicked, x + offset_x, y + offset_y);
        redraw_map();
    }
    else if (new_edge != -1) {
        mouse_x = event -> x;
        mouse_y = event -> y;
        redraw_map();
    }
    return FALSE;
}

static void add_vertex_clicked (GtkButton *button, gpointer data) {
    int id = create_vertex(MAP_W / 2, H / 2, "New");
    gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(start_select), id + 1, "New");
    gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(destination_select), id + 1, "New");
    redraw_map();
}

static void add_edge_clicked (GtkMenuItem *item, gpointer data) {
    new_edge = right_clicked;
}

static void delete_edge_clicked (GtkMenuItem *item, gpointer data) {
    remove_edge(edge_clicked);
    redraw_map();
}

static void open_weight_dialog (GtkMenuItem *item, gpointer data) {
    GtkWidget *dialog;
    GtkWidget *content;
    GtkWidget *entry;

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons("Edit weight", GTK_WINDOW(window), flags,
                                        "OK", GTK_RESPONSE_ACCEPT,
                                        "Cancel", GTK_RESPONSE_REJECT, NULL);
    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    entry = gtk_entry_new();
    sprintf(buffer, "%d", edge_clicked -> c);
    gtk_entry_set_text(GTK_ENTRY(entry), buffer);
    gtk_container_add(GTK_CONTAINER(content), entry);

    g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK (weight_dialog_response), entry);

    gtk_widget_show_all(dialog);
}

static void open_label_dialog (GtkMenuItem *item, gpointer data) {
    GtkWidget *dialog;
    GtkWidget *content;
    GtkWidget *entry;

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons("Edit label", GTK_WINDOW(window), flags,
                                        "OK", GTK_RESPONSE_ACCEPT,
                                        "Cancel", GTK_RESPONSE_REJECT, NULL);
    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), graph[right_clicked] -> label);
    gtk_container_add(GTK_CONTAINER(content), entry);

    g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK (label_dialog_response), entry);

    gtk_widget_show_all(dialog);
}

static void destroy_vertex_clicked (GtkMenuItem *item, gpointer data) {
    gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(start_select), right_clicked + 1);
    gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(destination_select), right_clicked + 1);
    remove_vertex(right_clicked);
    redraw_map();
}

static void label_dialog_response (GtkDialog *dialog, gint response_id, gpointer data) {
    GtkWidget *entry = data;
    if(response_id == GTK_RESPONSE_ACCEPT) {
        sprintf(graph[right_clicked] -> label, "%s", gtk_entry_get_text(GTK_ENTRY(entry)));
        gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(start_select), right_clicked + 1);
        gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(start_select), right_clicked + 1, graph[right_clicked] -> label);
        gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(destination_select), right_clicked + 1);
        gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(destination_select), right_clicked + 1, graph[right_clicked] -> label);
    }
    redraw_map();
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void weight_dialog_response (GtkDialog *dialog, gint response_id, gpointer data) {
    GtkWidget *entry = data;
    if(response_id == GTK_RESPONSE_ACCEPT) {
        edge_clicked -> c = atoi(gtk_entry_get_text(GTK_ENTRY(entry)));
    }
    redraw_map();
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void redraw_map () {
    gtk_widget_queue_draw_area(drawing_area, 0, 0, 2 * MAP_W, 2 * H);
}

static void find_route_clicked (GtkButton *button, gpointer data) {
    char *start_label = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(start_select));
    char *destination_label = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(destination_select));
    int start = find_vertex_with_label(start_label);
    int end = find_vertex_with_label(destination_label);
    if(start == -1 || end == -1) return;
    if(!calculate_route(start, end)) {
        GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
        GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(window), flags,
                                 GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                 "No route from '%s' to '%s' exists or the graph contains a negative weight cycle.\n",
                                 start_label, destination_label);

        g_signal_connect_swapped (G_OBJECT(dialog), "response", G_CALLBACK (gtk_widget_destroy), dialog);
        gtk_widget_show_all(dialog);
    }
    redraw_map();
}
static void clear_route_clicked (GtkButton *button, gpointer data) {
    clear_route();
    redraw_map();
}

static void save_map_clicked (GtkMenuItem *item, gpointer data) {
    save_map_state();
}

static void load_map_clicked (GtkMenuItem *item, gpointer data) {
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(start_select));
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(destination_select));
    gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(start_select), 0, "Select start");
    gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(destination_select), 0, "Select start");
    load_map_state();
    redraw_map();
    for(int i = 0; i < MAX_VERTICES; i++) {
        if(graph[i] == NULL) continue;
        gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(start_select), i + 1, graph[i] -> label);
        gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(destination_select), i + 1, graph[i] -> label);
    }
}
