#ifndef LIST
#define LIST

typedef struct list list;
struct list {
    int a;
    int b;
    int c;
    int on_route;
    list *tail;
};

void push_front (list **a, int x, int y, int z);

#endif
