/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

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
