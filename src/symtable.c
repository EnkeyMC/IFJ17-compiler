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
#include "memory_manager.h"

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
	HashTable* htab= (HashTable*) mm_malloc(sizeof(HashTable) + bucket_count*sizeof(htab_item*));
	if (htab == NULL)
		return NULL;
	htab->bucket_count = bucket_count;

	for (size_t i = 0; i < bucket_count; i++)
		htab->ptr[i] = NULL;

	return htab;
}

/**
 * Free all items contained in the hash table
 * @param htab Pointer to hash table
 */
static void htab_clear(HashTable *htab, bool func) {
	if (htab == NULL)
		return;
	for (size_t i = 0; i < htab->bucket_count; i++) {
		htab_item *prev;
		htab_item *next;
		for (prev = htab->ptr[i]; prev != NULL; prev = next) {
			next = prev->next;
			mm_free(prev->key);
			if (func) {
				buffer_free(prev->function->param_types);
				buffer_free(prev->function->param_names);
				mm_free(prev->function);
			}
			else
				mm_free(prev->variable);
			mm_free(prev);
		}
	}
	mm_free(htab);
}

void htab_free(void* htab) {
	htab_clear((HashTable*)htab, false);
}

void htab_func_free(HashTable* htab) {
	htab_clear(htab, true);
}

htab_item* htab_find(HashTable *htab, const char *key) {
	if (htab == NULL || key == NULL)
		return NULL;

	unsigned long index = hash_func(key) % htab->bucket_count;

	htab_item * item = htab->ptr[index];
	while (item != NULL) {
		if (strcmp(key, item->key) == 0)
			return item;
		else
			item = item->next;
	}
	return NULL;
}

static bool htab_remove_item(HashTable *htab, const char *key, bool func) {
	if (htab == NULL || key == NULL)
		return false;

	unsigned long index = hash_func(key) % htab->bucket_count;

	htab_item ** item = &(htab->ptr[index]);
	while (*item != NULL) {
		if (strcmp(key, (*item)->key) == 0)
			break;
		item = &((*item)->next);
	}
	
	if (*item == NULL)	// Item not found
		return false;

	htab_item * tmp = *item;
	*item = (*item)->next;

	mm_free(tmp->key);
	if (func) {
		buffer_free(tmp->function->param_types);
		buffer_free(tmp->function->param_names);
		mm_free(tmp->function);
	}
	else
		mm_free(tmp->variable);

	mm_free(tmp);
	return true;
}

bool htab_remove(HashTable* htab, const char *key) {
	return htab_remove_item(htab, key, false);
}

bool htab_func_remove(HashTable* htab, const char *key) {
	return htab_remove_item(htab, key, true);
}

/**
 * Alloc memory for function data stored in Hash Table item
 * @param item Hash table item
 * @return true on succes, false if allocation fails
 */
static bool alloc_func_item(htab_item* item) {
	item->function = (htab_function_item*) mm_malloc(sizeof(htab_function_item));
	if (item->function == NULL) {
		mm_free(item->key);
		mm_free(item);
		return false;
	}
	item->function->param_types = buffer_init(BUFFER_INIT_SIZE);
	if (item->function->param_types == NULL) {
		mm_free(item->key);
		mm_free(item->function);
		mm_free(item);
		return false;
	}
	item->function->param_names = buffer_init(BUFFER_INIT_SIZE);
	if (item->function->param_names == NULL) {
		mm_free(item->key);
		buffer_free(item->function->param_types);
		mm_free(item->function);
		mm_free(item);
		return false;
	}
	item->function->ret_type = END_OF_TERMINALS;
	item->function->params_num = 0;
	item->function->defined = false;

	return true;
}

static htab_item * htab_add_item(HashTable *htab, const char *key, bool func) {
	if (htab == NULL || key == NULL)
		return NULL;

	unsigned long index = hash_func(key) % htab->bucket_count;

	// Item contains address of pointer to next item
	htab_item ** item = &(htab->ptr[index]);
	while (*item != NULL) {
		if (strcmp(key, (*item)->key) == 0) {
			return *item;
		}
		else
			item = &((*item)->next);
	}

	// Allocate memory for new item
	htab_item* new_item = (htab_item*) mm_malloc(sizeof(htab_item));
	if (new_item == NULL)
		return NULL;

	// Alllocate memory for the key
	size_t key_length = strlen(key) + 1;
	new_item->key = (char*) mm_malloc(sizeof(char) * key_length);
	if (new_item->key == NULL) {
		mm_free(new_item);
		return NULL;
	}
	strncpy(new_item->key, key, key_length); // Copy the key into the new item

	// Allocate memory for item data
	if (func) {
		if (! alloc_func_item(new_item))
			return NULL;
	}
	else {
		new_item->variable = (htab_variable_item*) mm_malloc(sizeof(htab_variable_item));
		if (new_item->variable == NULL) {
			mm_free(new_item->key);
			mm_free(new_item);
			return NULL;
		}
		new_item->variable->type = END_OF_TERMINALS;
	}

	new_item->next = NULL;

	// Put the new item at the end of the list of items
	*item = new_item;

	return new_item;
}

htab_item * htab_lookup(HashTable* htab, const char* key) {
	return htab_add_item(htab, key, false);
}

htab_item * htab_func_lookup(HashTable* htab, const char* key) {
	return htab_add_item(htab, key, true);
}

void htab_foreach(HashTable *htab, void (*function)(htab_item *item)) {
	if (htab == NULL || function == NULL)
		return;

	for (size_t i = 0; i < htab->bucket_count; i++)
		for (htab_item *item = htab->ptr[i]; item != NULL; item = item->next)
			function(item);
}

void variable_item_debug(htab_item * item) {
	debug("htab_item@%p: {", item);

	if (item != NULL) {
		debug(".key = %s, .type: %d", item->key, item->variable->type);
	}

	debugs("}\n");
}

void function_item_debug(htab_item * item) {
	debug("htab_item@%p: {", item);

	if (item != NULL) {
		debug(".key = %s, .rt = %d", item->key, item->function->ret_type);
		debug(", .nparams = %u, .types = %s",
				item->function->params_num,
				item->function->param_types->str);
		debug(", .defined = %s", item->function->defined ? "true" : "false");
	}

	debugs("}\n");
}

// --------------------------------------------------------------------
// FUNCTIONS TO MODIFY/ACCESS HASH TABLE ITEMS (and their 'attributes')
// --------------------------------------------------------------------

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
	item->function->params_num++;
	return buffer_append_c(item->function->param_types, c);
}

token_e func_get_param(htab_item* item, unsigned idx) {
	assert(item != NULL);
	assert(idx != 0);

	if (idx > item->function->params_num)
		return END_OF_TERMINALS;

	char param_type = item->function->param_types->str[idx-1];
	switch (param_type) {
		case 's': return TOKEN_KW_STRING;
		case 'b': return TOKEN_KW_BOOLEAN;
		case 'i': return TOKEN_KW_INTEGER;
		case 'd': return TOKEN_KW_DOUBLE;
		default: return END_OF_TERMINALS;
	}
}

unsigned func_get_param_idx(htab_item* item) {
	assert(item != NULL);

	unsigned num = 1;
	for (size_t i = 0; i <= item->function->param_names->len; i++) {
		if (num > item->function->params_num)
			break;
		if (item->function->param_names->str[i] == '#')
			num++;
	}
	return num;
}

char* func_get_param_name(htab_item* item, unsigned idx) {
	assert(item != NULL);
	assert(idx != 0);

	if (idx > item->function->params_num)
		return NULL;

	unsigned markers_found = 0;
	char* start = item->function->param_names->str;
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

	char* param_name = (char*) mm_malloc(sizeof(char) * (len + 1));
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

	if (buffer_append_str(item->function->param_names, name))
		return buffer_append_c(item->function->param_names, '#');
	return false;
}

unsigned int func_get_params_num(htab_item* item) {
	assert(item != NULL);

	return item->function->params_num;
}

void func_set_ret_type(htab_item* item, token_e type) {
	assert(item != NULL);

	item->function->ret_type = type;
}

token_e func_get_ret_type(htab_item* item) {
	assert(item != NULL);

	return item->function->ret_type;
}

void func_set_defined(htab_item* item) {
	assert(item != NULL);

	item->function->defined = true;
}

bool func_get_defined(htab_item* item) {
	assert(item != NULL);

	return item->function->defined;
}

bool func_check_all_defined(HashTable *htab) {
	assert(htab != NULL);

	for (size_t i = 0; i < htab->bucket_count; i++)
		for (htab_item *item = htab->ptr[i]; item != NULL; item = item->next) {
			if (item->function->defined == false)
				return false;
		}
	return true;
}

void var_set_type(htab_item* item, token_e type) {
	assert(item != NULL);

	item->variable->type = type;
}

token_e var_get_type(htab_item* item) {
	assert(item != NULL);

	return item->variable->type;
}
