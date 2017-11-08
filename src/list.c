#include <malloc.h>
#include <assert.h>
#include "list.h"

List* list_init(free_data_f free_data) {
	List* l = (List*) malloc(sizeof(List));
	if (l == NULL) {
		return NULL;
	}

	l->first = NULL;
	l->active = NULL;
	l->free_data = free_data;

	return l;
}

void list_free(List* l) {
	assert(l != NULL);

	if (l->free_data != NULL) {
		list_activate(l);
		while (list_active(l)) {
			l->free_data(list_delete_active(l));
		}
	}

	free(l);
}

bool list_insert_first(List* l, void* data) {
	assert(l != NULL);

	ListItem* new_item = (ListItem*) malloc(sizeof(ListItem));
	if (new_item == NULL) {
		return false;
	}

	new_item->data = data;
	new_item->next = l->first;

	l->first = new_item;
	return true;
}

bool list_post_insert(List* l, void* data) {
	assert(l != NULL);

	if (l->active == NULL)
		return false;

	ListItem* new_item = (ListItem*) malloc(sizeof(ListItem));
	if (new_item == NULL) {
		return false;
	}

	new_item->data = data;
	new_item->next = l->active->next;

	l->active->next = new_item;

	return true;
}

bool list_empty(List* l) {
	assert(l != NULL);

	return l->first == NULL;
}

bool list_active(List* l) {
	assert(l != NULL);

	return l->active != NULL;
}

void* list_get_active(List* l) {
	assert(l != NULL);

	if (l->active == NULL)
		return NULL;

	return l->active->data;
}

void* list_get_first(List* l) {
	assert(l != NULL);

	if (l->first == NULL)
		return NULL;

	return l->active->data;
}

void* list_delete_frist(List* l) {
	assert(l != NULL);

	if (l->first == NULL)
		return NULL;

	void* data = l->first->data;
	ListItem* to_delete = l->first;
	l->first = l->first->next;

	free(to_delete);
	return data;
}

void* list_delete_active(List* l) {
	assert(l != NULL);

	if (l->active == NULL)
		return NULL;

	void* data = l->active->data;
	ListItem* to_delete = l->active;

	if (l->first == l->active) {
		l->first = l->active->next;
	}

	l->active = l->active->next;

	free(to_delete);
	return data;
}

void list_activate(List* l) {
	assert(l != NULL);

	l->active = l->first;
}

void list_succ(List* l) {
	assert(l != NULL);

	l->active = l->active->next;
}

int list_length(List* l) {
	assert(l != NULL);

	int length = 0;
	list_activate(l);
	while (list_active(l)) {
		length++;
		list_succ(l);
	}

	return length;
}
