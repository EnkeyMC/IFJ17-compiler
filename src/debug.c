#include "debug.h"

void int_debug(void* i) {
	if (i == NULL) {
		debug("%p", i);
	} else {
		debug("%d", *(int*) i);
	}
}

void double_debug(void* d) {
	if (d == NULL) {
		debug("%p", d);
	} else {
		debug("%g", *(double*) d);
	}
}
