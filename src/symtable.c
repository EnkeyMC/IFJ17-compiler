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

static HashTable* htab_initialize(size_t bucket_count) {
	HashTable* htab_ptr = (HashTable*) malloc(sizeof(HashTable) + bucket_count*sizeof(htab_item*));
	if (htab_ptr == NULL)
		return NULL;
	htab_ptr->bucket_count = bucket_count;
	htab_ptr->size = 0;

	for (size_t i = 0; i < bucket_count; i++)
		htab_ptr->ptr[i] = NULL;

	return htab_ptr;
}

HashTable* htab_init(size_t bucket_count) {
	return htab_initialize(bucket_count);
}

HashTable *htab_func_init(size_t bucket_count) {
	HashTable* htab_ptr = htab_initialize(bucket_count);
	if (htab_ptr == NULL)
		return NULL;

	// Add built-in functions
	const char *built_in_func[] = { "length", "substr", "asc", "chr" };
	const char *built_in_params[]= { "s", "sii", "si", "i" };
	token_e built_in_rts[]= {	// Return types
		TOKEN_KW_INTEGER, TOKEN_KW_STRING, TOKEN_KW_INTEGER, TOKEN_KW_STRING
	};
	htab_item* built_in;
	for (int i = 0; i < 4; i++) {
		built_in = htab_func_lookup(htab_ptr, built_in_func[i]);
		if (built_in == NULL) {
			htab_func_free(htab_ptr);
			return NULL;
		}
		func_set_rt(built_in, built_in_rts[i]);
		func_set_def(built_in);
		for (int k = 0; built_in_params[i][k] != '\0'; k++) {
			if (! func_add_param(built_in, built_in_params[i][k])) {
				htab_func_free(htab_ptr);
				return NULL;
			}
		}
	}

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
				buffer_free(prev->func_data->params_buff);
				dllist_free(prev->func_data->sym_table_stack);
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
	htab_clear(((HashTable*)htab_ptr), false);
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
		buffer_free(tmp->func_data->params_buff);
		dllist_free(tmp->func_data->sym_table_stack);
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
	item->func_data->params_buff = buffer_init(BUFFER_INIT_SIZE);
	if (item->func_data->params_buff == NULL) {
		free(item->key);
		free(item->func_data);
		free(item);
		return false;
	}
	item->func_data->sym_table_stack = dllist_init(htab_free);
	if (item->func_data->sym_table_stack == NULL) {
		free(item->key);
		buffer_free(item->func_data->params_buff);
		free(item->func_data);
		free(item);
		return false;
	}
	item->func_data->rt = END_OF_TERMINALS;
	item->func_data->params_num = 0;
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

void print_id_item(htab_item * item_ptr) {
	printf("Key: '%s'\tType: %d\n", item_ptr->key, item_ptr->id_data->type);
}

void print_func_item(htab_item * item_ptr) {
	printf("Key: '%s'\tReturn type: %d\n", item_ptr->key, item_ptr->func_data->rt);
	printf("\tNumber of params: '%u'\tParam types: %s\n", item_ptr->func_data->params_num, item_ptr->func_data->params_buff->str);
	printf("\tDefinition provided?: '%s'\n", item_ptr->func_data->definition ? "true" : "false");
	printf("---------------------------------------------------------------'\n");
}

// FUNCTIONS TO MODIFY FUNCTION DATA STORED IN HASH TABLE

bool func_add_param(htab_item* item, char c) {
	assert(item != NULL);

	item->func_data->params_num++;
	// Stores parameter types in following format: "p1#p2#p3#"
	// where p1..p3 is one of { i, s, d, b }
	if (buffer_append_c(item->func_data->params_buff, c))
		return buffer_append_c(item->func_data->params_buff, '#');
	return false;
}

token_e func_get_param(htab_item* item, unsigned idx) {
	assert(item != NULL);

	if (idx > item->func_data->params_num)
		return END_OF_TERMINALS;

	char param_type = item->func_data->params_buff->str[idx*2 -2];
	switch (param_type) {
		case 's': return TOKEN_KW_STRING;
		case 'b': return TOKEN_KW_BOOLEAN;
		case 'i': return TOKEN_KW_INTEGER;
		case 'd': return TOKEN_KW_DOUBLE;
		default: return END_OF_TERMINALS;
	}
}

void func_set_rt(htab_item* item, token_e type) {
	assert(item != NULL);

	item->func_data->rt = type;
}

void func_set_def(htab_item* item) {
	assert(item != NULL);

	item->func_data->definition = true;
}

HashTable* func_new_scope(htab_item* item) {
	assert(item != NULL);

	// Create new local symtable
	HashTable* local = htab_init(HTAB_INIT_SIZE);
	if (local == NULL)
		return NULL;

	// Push it on top of the stack
	if (! dllist_insert_first(item->func_data->sym_table_stack, local)) {
		free(local);
		return NULL;
	}

	return local;
}
