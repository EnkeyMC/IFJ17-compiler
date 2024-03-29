/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <malloc.h>
#include <assert.h>
#include "dllist.h"
#include "memory_manager.h"

DLList* dllist_init(free_data_f free_data) {
	DLList* l = (DLList*) mm_malloc(sizeof(DLList));

	l->first = NULL;
	l->last = NULL;
	l->active = NULL;
	l->free_data = free_data;

	return l;
}

void dllist_free(DLList *l) {
	assert(l != NULL);

	dllist_activate_first(l);
	while (dllist_active(l)) {
		if (l->free_data != NULL) {
			l->free_data(dllist_delete_and_succ(l));
		} else {
			dllist_delete_and_succ(l);
		}
	}

	mm_free(l);
}

DLList* dllist_copy(DLList *l) {
	assert(l != NULL);

	DLList *l_copy = dllist_init(l->free_data);

	dllist_activate_first(l);
	if (dllist_active(l)) {
		dllist_insert_first(l_copy, dllist_get_active(l));
		dllist_succ(l);
		while (dllist_active(l)) {
			dllist_insert_last(l_copy, dllist_get_active(l));
			dllist_succ(l);
		}
	}
	return l_copy;
}

void dllist_insert_first(DLList *l, void *data) {
	assert(l != NULL);

	DLListItem* new_item = (DLListItem*) mm_malloc(sizeof(DLListItem));

	new_item->data = data;
	new_item->prev = NULL;

	if (l->first != NULL) {
		new_item->next = l->first;
		l->first->prev = new_item;
	} else {
		new_item->next = NULL;
		l->last = new_item;
	}

	l->first = new_item;
}

void dllist_insert_last(DLList *l, void *data) {
	assert(l != NULL);

	DLListItem* new_item = (DLListItem*) mm_malloc(sizeof(DLListItem));

	new_item->data = data;
	new_item->next = NULL;

	if (l->last != NULL) {
		new_item->prev = l->last;
		l->last->next = new_item;
	} else {
		new_item->prev = NULL;
		l->first = new_item;
	}

	l->last = new_item;
}

void dllist_post_insert(DLList *l, void *data) {
	assert(l != NULL);

	if (l->active == NULL)
		return;

	DLListItem* new_item = (DLListItem*) mm_malloc(sizeof(DLListItem));

	new_item->data = data;
	new_item->next = l->active->next;
	new_item->prev = l->active;

	if (l->active->next != NULL) {
		l->active->next->prev = new_item;
	} else {
		l->last = new_item;
	}

	l->active->next = new_item;
}

void dllist_pre_insert(DLList *l, void *data) {
	assert(l != NULL);

	if (l->active == NULL)
		return;

	DLListItem* new_item = (DLListItem*) mm_malloc(sizeof(DLListItem));

	new_item->data = data;
	new_item->next = l->active;
	new_item->prev = l->active->prev;

	if (l->active->prev != NULL) {
		l->active->prev->next = new_item;
	} else {
		l->first = new_item;
	}

	l->active->prev = new_item;
}

bool dllist_empty(DLList *l) {
	assert(l != NULL);

	return l->first == NULL;
}

bool dllist_active(DLList *l) {
	assert(l != NULL);

	return l->active != NULL;
}

void* dllist_get_active(DLList *l) {
	assert(l != NULL);

	if (l->active == NULL)
		return NULL;

	return l->active->data;
}

void* dllist_get_first(DLList *l) {
	assert(l != NULL);

	if (l->first == NULL)
		return NULL;

	return l->first->data;
}

void* dllist_get_last(DLList *l) {
	assert(l != NULL);

	if (l->last == NULL)
		return NULL;

	return l->last->data;
}

void* dllist_delete_first(DLList *l) {
	assert(l != NULL);

	if (l->first == NULL)
		return NULL;

	if (l->first == l->active)
		l->active = NULL;

	void* data = l->first->data;
	DLListItem* to_delete = l->first;
	l->first = l->first->next;

	if (l->first != NULL)
		l->first->prev = NULL;
	else
		l->last = NULL;

	mm_free(to_delete);
	return data;
}

void* dllist_delete_last(DLList *l) {
	assert(l != NULL);

	if (l->last == NULL)
		return NULL;

	if (l->last == l->active)
		l->active = NULL;

	void* data = l->last->data;
	DLListItem* to_delete = l->last;
	l->last = l->last->prev;

	if (l->last != NULL)
		l->last->next = NULL;
	else
		l->first = NULL;

	mm_free(to_delete);
	return data;	
}

void* dllist_delete_and_succ(DLList *l) {
	assert(l != NULL);

	if (l->active == NULL)
		return NULL;

	void* data = l->active->data;
	DLListItem* to_delete = l->active;

	if (l->first == l->active) {
		l->first = l->active->next;

		if (l->first != NULL) {
			l->first->prev = NULL;
		}
	}

	if (l->last == l->active) {
		l->last = l->active->prev;

		if (l->last != NULL) {
			l->last->next = NULL;
		}
	}

	if (l->active->prev != NULL)
		l->active->prev->next = l->active->next;
	if (l->active->next != NULL)
		l->active->next->prev = l->active->prev;

	l->active = l->active->next;

	mm_free(to_delete);
	return data;
}

void* dllist_delete_and_pred(DLList *l) {
	assert(l != NULL);

	if (l->active == NULL)
		return NULL;

	void* data = l->active->data;
	DLListItem* to_delete = l->active;

	if (l->first == l->active) {
		l->first = l->active->next;

		if (l->first != NULL) {
			l->first->prev = NULL;
		}
	}

	if (l->last == l->active) {
		l->last = l->active->prev;

		if (l->last != NULL) {
			l->last->next = NULL;
		}
	}

	if (l->active->prev != NULL)
		l->active->prev->next = l->active->next;
	if (l->active->next != NULL)
		l->active->next->prev = l->active->prev;

	l->active = l->active->prev;

	mm_free(to_delete);
	return data;
}

void* dllist_update(DLList *l, void* data) {
	assert(l != NULL);

	if (l->active == NULL)
		return NULL;

	void* old = l->active->data;
	l->active->data = data;
	return old;
}

void dllist_activate_first(DLList *l) {
	assert(l != NULL);

	l->active = l->first;
}

void dllist_activate_last(DLList *l) {
	assert(l != NULL);

	l->active = l->last;
}

void dllist_succ(DLList *l) {
	assert(l != NULL);

	l->active = l->active->next;
}

void dllist_pred(DLList *l) {
	assert(l != NULL);

	l->active = l->active->prev;
}

int dllist_length(DLList *l) {
	assert(l != NULL);

	int length = 0;
	dllist_activate_first(l);
	while (dllist_active(l)) {
		length++;
		dllist_succ(l);
	}

	return length;
}

void dllist_debug(void* l, debug_func func) {
	DLList* list = (DLList*) l;

	debug("DLList@%p: {\n", list);
	dllist_activate_first(list);
	while (dllist_active(list)) {
		debugs("\t");
		func(dllist_get_active(list));
		debugs("\n");
		dllist_succ(list);
	}
	debugs("}\n\n");
}

