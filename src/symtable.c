/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <assert.h>
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

HashTable* htab_init(size_t bucket_count) {
	HashTable* htab_ptr = (HashTable*) malloc(sizeof(HashTable) + bucket_count*sizeof(htab_item*));
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
static void htab_clear(HashTable *htab_ptr, bool func) {
	if (htab_ptr == NULL)
		return;
	for (size_t i = 0; i < htab_ptr->bucket_count; i++) {
		htab_item *prev;
		htab_item *next;
		for (prev = htab_ptr->ptr[i]; prev != NULL; prev = next) {
			next = prev->next;
			free(prev->key);
			if (func) {
				buffer_free(prev->func_data->par_types);
				buffer_free(prev->func_data->par_names);
				free(prev->func_data);
			}
			else
				free(prev->id_data);
			free(prev);
		}
	}
	free(htab_ptr);
}

void htab_free(void* htab_ptr) {
	htab_clear((HashTable*)htab_ptr, false);
}

void htab_func_free(HashTable* htab_ptr) {
	htab_clear(htab_ptr, true);
}

htab_item* htab_find(HashTable *htab_ptr, const char *key) {
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

static bool htab_remove_item(HashTable *htab_ptr, const char *key, bool func) {
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
	if (func) {
		buffer_free(tmp->func_data->par_types);
		buffer_free(tmp->func_data->par_names);
		free(tmp->func_data);
	}
	else
		free(tmp->id_data);

	free(tmp);
	return true;
}

bool htab_remove(HashTable* htab_ptr, const char *key) {
	return htab_remove_item(htab_ptr, key, false);
}

bool htab_func_remove(HashTable* htab_ptr, const char *key) {
	return htab_remove_item(htab_ptr, key, true);
}

/**
 * Alloc memory for function data stored in Hash Table item
 * @param item Hash table item
 * @return true on succes, false if allocation fails
 */
static bool alloc_func_item(htab_item* item) {
	item->func_data = (htab_func_item*) malloc(sizeof(htab_func_item));
	if (item->func_data == NULL) {
		free(item->key);
		free(item);
		return false;
	}
	item->func_data->par_types = buffer_init(BUFFER_INIT_SIZE);
	if (item->func_data->par_types == NULL) {
		free(item->key);
		free(item->func_data);
		free(item);
		return false;
	}
	item->func_data->par_names = buffer_init(BUFFER_INIT_SIZE);
	if (item->func_data->par_names == NULL) {
		free(item->key);
		buffer_free(item->func_data->par_types);
		free(item->func_data);
		free(item);
		return false;
	}
	item->func_data->rt = END_OF_TERMINALS;
	item->func_data->par_num = 0;
	item->func_data->definition = false;

	return true;
}

static htab_item * htab_add_item(HashTable *htab_ptr, const char *key, bool func) {
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
	htab_item* new_item = (htab_item*) malloc(sizeof(htab_item));
	if (new_item == NULL)
		return NULL;

	// Alllocate memory for the key
	size_t key_length = strlen(key) + 1;
	new_item->key = (char*) malloc(sizeof(char) * key_length);
	if (new_item->key == NULL) {
		free(new_item);
		return NULL;
	}
	strncpy(new_item->key, key, key_length); // Copy the key into the new item

	// Allocate memory for item data
	if (func) {
		if (! alloc_func_item(new_item))
			return NULL;
	}
	else {
		new_item->id_data = (htab_id_item*) malloc(sizeof(htab_id_item));
		if (new_item->id_data == NULL) {
			free(new_item->key);
			free(new_item);
			return NULL;
		}
		new_item->id_data->type = END_OF_TERMINALS;
	}

	new_item->next = NULL;

	// Put the new item at the end of the list of items
	*item = new_item;

	// Update the number of items
	htab_ptr->size++;

	return new_item;
}

htab_item * htab_lookup(HashTable* htab_ptr, const char* key) {
	return htab_add_item(htab_ptr, key, false);
}

htab_item * htab_func_lookup(HashTable* htab_ptr, const char* key) {
	return htab_add_item(htab_ptr, key, true);
}

size_t htab_size(HashTable *htab_ptr) {
	return htab_ptr != NULL ? htab_ptr->size : 0;
}

size_t htab_bucket_count(HashTable *htab_ptr) {
	return htab_ptr != NULL ? htab_ptr->bucket_count : 0;
}

void htab_foreach(HashTable *htab_ptr, void (*function)(htab_item *item_ptr)) {
	if (htab_ptr == NULL || function == NULL)
		return;

	for (size_t i = 0; i < htab_ptr->bucket_count; i++)
		for (htab_item *item = htab_ptr->ptr[i]; item != NULL; item = item->next)
			function(item);
}

void id_item_debug(void * item_ptr) {
	htab_item* item = (htab_item*) item_ptr;
	debug("htab_item@%p: {", item);

	if (item != NULL) {
		debug(".key = %s, .type: %d", item->key, item->id_data->type);
	}

	debug("}");
}

void func_item_debug(void * item_ptr) {
	htab_item* item = (htab_item*) item_ptr;
	debug("htab_item@%p: {", item);

	if (item != NULL) {
		debug(".key = %s, .rt = %d", item->key, item->func_data->rt);
		printf(", .nparams = %u, .types = %s", item->func_data->par_num, item->func_data->par_types->str);
		printf(", .defined = %s", item->func_data->definition ? "true" : "false");
	}

	debug("}");
}

// FUNCTIONS TO MODIFY FUNCTION DATA STORED IN HASH TABLE

bool func_add_param(htab_item* item, token_e type) {
	assert(item != NULL);

	char c;
	switch (type) {
		case TOKEN_KW_INTEGER:
			c = 'i';
			break;
		case TOKEN_KW_DOUBLE:
			c = 'd';
			break;
		case TOKEN_KW_STRING:
			c = 's';
			break;
		case TOKEN_KW_BOOLEAN:
			c = 'b';
			break;
		default: return false;
	}
	item->func_data->par_num++;
	return buffer_append_c(item->func_data->par_types, c);
}

token_e func_get_param(htab_item* item, unsigned idx) {
	assert(item != NULL);
	assert(idx != 0);

	if (idx > item->func_data->par_num)
		return END_OF_TERMINALS;

	char param_type = item->func_data->par_types->str[idx-1];
	switch (param_type) {
		case 's': return TOKEN_KW_STRING;
		case 'b': return TOKEN_KW_BOOLEAN;
		case 'i': return TOKEN_KW_INTEGER;
		case 'd': return TOKEN_KW_DOUBLE;
		default: return END_OF_TERMINALS;
	}
}

char* func_get_param_name(htab_item* item, unsigned idx) {
	assert(item != NULL);
	assert(idx != 0);

	if (idx > item->func_data->par_num)
		return NULL;

	unsigned markers_found = 0;
	char* start = item->func_data->par_names->str;
	while (markers_found != (idx -1)) {
		start++;
		if (*start == '#') {
			start++;
			markers_found++;
		}
	}

	unsigned len = 0;
	while (*(start + len) != '#')
		len++;

	char* param_name = (char*) malloc(sizeof(char) * (len + 1));
	if (param_name == NULL)
		return NULL;

	if (! strncpy(param_name, start, len))
		return NULL;
	param_name[len] = '\0';
	return param_name;
}

bool func_store_param_name(htab_item* item, const char* name) {
	assert(item != NULL);
	assert(name != NULL);

	if (buffer_append_str(item->func_data->par_names, name))
		return buffer_append_c(item->func_data->par_names, '#');
	return false;
}

void func_set_rt(htab_item* item, token_e type) {
	assert(item != NULL);

	item->func_data->rt = type;
}

token_e func_get_rt(htab_item* item) {
	assert(item != NULL);

	return item->func_data->rt;
}

void func_set_def(htab_item* item) {
	assert(item != NULL);

	item->func_data->definition = true;
}
