#include "list.h"
#include <stdlib.h>

void push_front (list **a, int x, int y, int z) {
    list *b = (list*) malloc (sizeof(list));
    b -> a = x;
    b -> b = y;
    b -> c = z;
    b -> on_route = 0;
    b -> tail = *a;
    *a = b;
}
