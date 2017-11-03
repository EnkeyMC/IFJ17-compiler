#include <malloc.h>
#include <assert.h>
#include "sparse_table.h"

SparseTable* sparse_table_init(unsigned int nrows, unsigned int ncols, int dominant_value) {
	SparseTable* stab = (SparseTable*) malloc(sizeof(SparseTable));
	if (stab == NULL)
		return NULL;

	stab->nrows = nrows;
	stab->ncols = ncols;
	stab->dominant_value = dominant_value;

	stab->table = (SparseTableListItem**) malloc(sizeof(SparseTableListItem*) * nrows);
	if (stab->table == NULL) {
		free(stab);
		return NULL;
	}

	for (int i = 0; i < nrows; i++) {
		stab->table[i] = NULL;
	}

	return stab;
}

void sparse_table_free(SparseTable* stab) {
	assert(stab != NULL);

	if (stab->table != NULL) {
		SparseTableListItem* tmp;
		for (int i = 0; i < stab->nrows; i++) {
			while (stab->table[i] != NULL) {
				tmp = stab->table[i];
				stab->table[i] = tmp->next;
				free(tmp);
			}
		}

		free(stab->table);
	}
	free(stab);
}

int sparse_table_get(SparseTable* stab, unsigned int row, unsigned int column) {
	assert(stab != NULL);

	if (row >= stab->nrows || column >= stab->ncols) {
		return stab->dominant_value;
	}

	SparseTableListItem* curr_item = stab->table[row];

	while (curr_item != NULL) {
		if (curr_item->column == column) {
			return curr_item->value;
		}

		curr_item = curr_item->next;
	}

	return stab->dominant_value;
}

bool sparse_table_set(SparseTable* stab, unsigned int row, unsigned int column, int value) {
	assert(stab != NULL);

	if (row >= stab->nrows || column >= stab->ncols) {
		return false;
	}

	SparseTableListItem* last_item = stab->table[row];

	if (last_item != NULL) {
		SparseTableListItem* tmp = last_item;
		do {
			last_item = tmp;
			if (column == last_item->column) {
				last_item->value = value;
				return true;
			}

			tmp = last_item->next;
		} while (tmp != NULL);

		last_item->next = (SparseTableListItem*) malloc(sizeof(SparseTableListItem));
		if (last_item->next == NULL)
			return false;

		last_item->next->next = NULL;
		last_item->next->value = value;
		last_item->next->column = column;
	} else {
		stab->table[row] = (SparseTableListItem*) malloc(sizeof(SparseTableListItem));
		if (stab->table[row] == NULL)
			return false;

		stab->table[row]->next = NULL;
		stab->table[row]->value = value;
		stab->table[row]->column = column;
	}
	return true;
}
