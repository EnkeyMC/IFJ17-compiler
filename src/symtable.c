/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"

/**
 * Hash function djb2 (http://www.cse.yorku.ca/~oz/hash.html)
 * @param str String to convert into hash value
 * @return Hash value
 */
static unsigned long hash_func(const char *str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;

	return hash;
}

HashTable *htab_init(size_t bucket_count) {
	HashTable *htab_ptr = (HashTable*) malloc(sizeof(HashTable) + bucket_count*sizeof(htab_item));
	if (htab_ptr == NULL)
		return NULL;
	htab_ptr->bucket_count = bucket_count;
	htab_ptr->size = 0;

	for (size_t i = 0; i < bucket_count; i++)
		htab_ptr->ptr[i] = NULL;

	return htab_ptr;
}

/**
 * Free all items contained in the hash table
 * @param htab_ptr Pointer to hash table
 */
static void htab_clear(HashTable *htab_ptr) {
	for (size_t i = 0; i < htab_ptr->size; i++) {
		htab_item *prev;
		htab_item *next;
		for (prev = htab_ptr->ptr[i]; prev != NULL; prev = next) {
			next = prev->next;
			free(prev->key);
			free(prev);
		}
	}
}

void htab_free(HashTable *htab_ptr) {
	if (htab_ptr == NULL)
		return;
	htab_clear(htab_ptr);
	free(htab_ptr);
}

htab_item * htab_find(HashTable *htab_ptr, const char *key) {
	if (htab_ptr == NULL || key == NULL)
		return NULL;

	unsigned long index = hash_func(key) % htab_ptr->bucket_count;

	htab_item * item = htab_ptr->ptr[index];
	while (item != NULL) {
		if (strcmp(key, item->key) == 0)
			return item;
		else
			item = item->next;
	}
	return NULL;
}

bool htab_remove(HashTable *htab_ptr, const char *key) {
	if (htab_ptr == NULL || key == NULL)
		return false;

	unsigned long index = hash_func(key) % htab_ptr->bucket_count;

	htab_item ** item = &(htab_ptr->ptr[index]);
	while (*item != NULL) {
		if (strcmp(key, (*item)->key) == 0)
			break;
		item = &((*item)->next);
	}
	
	if (*item == NULL)	// Item not found
		return false;

	htab_item * tmp = *item;
	*item = (*item)->next;

	free(tmp->key);
	free(tmp);
	return true;
}

htab_item * htab_lookup(HashTable *htab_ptr, const char *key) {
	if (htab_ptr == NULL || key == NULL)
		return NULL;

	unsigned long index = hash_func(key) % htab_ptr->bucket_count;

	// Item contains address of pointer to next item
	htab_item ** item = &(htab_ptr->ptr[index]);
	while (*item != NULL) {
		if (strcmp(key, (*item)->key) == 0) {
			return *item;
		}
		else
			item = &((*item)->next);
	}

	// Allocate memory for new item
	htab_item * new_item = (htab_item*) malloc(sizeof(htab_item));
	if (new_item == NULL)
		return NULL;

	// Alllocate memory for the key
	size_t key_length = strlen(key) + 1;
	new_item->key = (char*) malloc(sizeof(char) * key_length);
	if (new_item->key == NULL) {
		free(new_item);
		return NULL;
	}

	// Copy the key into the new item
	strncpy(new_item->key, key, key_length);
	new_item->next = NULL;
	new_item->type = END_OF_TERMINALS;

	// Put the new item at the end of the list of items
	*item = new_item;

	// Update the number of items
	htab_ptr->size++;

	return new_item;
}
size_t htab_size(HashTable *htab_ptr) {
	return htab_ptr != NULL ? htab_ptr->size : 0;
}

size_t htab_bucket_count(HashTable *htab_ptr) {
	return htab_ptr != NULL ? htab_ptr->bucket_count : 0;
}

void htab_foreach(HashTable *htab_ptr, void (*func)(htab_item *item_ptr)) {
	if (htab_ptr == NULL || func == NULL)
		return;

	for (size_t i = 0; i < htab_ptr->bucket_count; i++)
		for (htab_item *item = htab_ptr->ptr[i]; item != NULL; item = item->next)
			func(item);
}

void print_item(htab_item * item_ptr) {
	printf("Key: '%s'\tType: %d\n", item_ptr->key, item_ptr->type);
}
