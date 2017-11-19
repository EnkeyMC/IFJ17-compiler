/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#ifndef IFJ17_COMPILER_SYMTABLE_H
#define IFJ17_COMPILER_SYMTABLE_H

#include <stddef.h>
#include <stdbool.h>
#include "token.h"
#include "dllist.h"
#include "buffer.h"

#define BUFFER_INIT_SIZE 12
#define HTAB_INIT_SIZE 67

/**
 * Function hash table item
 */
typedef struct htab_func_item_t {
	token_e rt;	/// Return type
	unsigned params_num;	/// Number of parameters
	Buffer* params_buff;	/// String containing paramater types
	bool definition;	/// Was already defined?
} htab_func_item;

typedef struct htab_id_item_t {
	token_e type;	/// Identifier type
} htab_id_item;

/**
 * Identifier hash table item
 */
typedef struct htab_item_t {
	char *key;	/// Name of identifier
	struct htab_item_t * next;	/// Pointer to next item in the list
	union {
		htab_func_item* func_data;
		htab_id_item* id_data;
	};
} htab_item;

/**
 * Hash Table structure
 */
typedef struct hash_table {
	size_t bucket_count;	/// Number of buckets contained in the hash table
	size_t size;	/// Number of all items. Useful for testing
	htab_item *ptr[];	/// Array(of size 'bucket_count') of buckets
} HashTable;

/**
 * Initialize empty hash table
 * @param bucket_count Size of array of buckets
 * @return Pointer to empty hash table or NULL if allocation of memory failed
 */
HashTable *htab_init(size_t bucket_count);

/**
 * Free whole hash table from memory
 * @param htab_ptr Pointer to hash table
 */
void htab_free(void* htab_ptr);
void htab_func_free(HashTable *htab_ptr);

/**
 * Find item
 * @param htab_ptr Pointer to hash table
 * @param key String identifying an item
 * @return Pointer to item or NULL if the item does not exist
 */
htab_item * htab_find(HashTable *htab_ptr, const char* key);

/**
 * Remove given bucket
 * @param htab_ptr Pointer to hash table
 * @param key String identifying an item
 * @return true on seccuss or false if the item does not exist
 */
bool htab_remove(HashTable *htab_ptr, const char *key);
bool htab_func_remove(HashTable *htab_ptr, const char *key);

/**
 * Find existing bucket or add new one if it does not exist
 * @param htab_ptr Pointer to hash table
 * @param key String identifying an item
 * @return Pointer to item or NULL if allocation of memory for new item fails
 */
htab_item* htab_lookup(HashTable *htab_ptr, const char* key);
htab_item* htab_func_lookup(HashTable *htab_ptr, const char* key);

/**
 * Get the number of buckets
 * @param htab_ptr Pointer to hash table
 * @return Number of buckets
 */
size_t htab_bucket_count(HashTable *htab_ptr);

/**
 * Get hash table size
 * @param htab_ptr Pointer to hash table
 * @return Number of all entries/items in the hash table
 */
size_t htab_size(HashTable *htab_ptr);

/**
 * For each entry in the hash table call function given as 2nd argument
 * @param htab_ptr Pointer to hash table
 * @param func Pointer to function which takes pointer to hash table item as argument
 */
void htab_foreach(HashTable *htab_ptr, void (*func)(htab_item *item_ptr));

/**
 * Print contents of an identifier/function data to stdout. Use with htab_foreach in first place.
 * @param item_ptr Pointer to hash table item
 */
void print_id_item(htab_item *item_ptr);
void print_func_item(htab_item *item_ptr);

// FUNCTIONS TO MODIFY FUNCTION DATA STORED IN HASH TABLE
/**
 * Add parameter type to function record in hash table
 * @param item Pointer to hash table item (that stores function data)
 * @param type parameter data type
 * @return true on success, false otherwise
 */
bool func_add_param(htab_item* item, token_e type);

/**
 * Get type of function parameter from given index
 * @param item Pointer to hash table item (that stores function data)
 * @param idx Parameter index - INDEXING START AT 1 !!!
 * @return Parameter data type (e.g. TOKEN_KW_STRING, TOKEN_KW_INTEGER,...) on success, END_OF_TERMINALS otherwise
 */
token_e func_get_param(htab_item* item, unsigned idx);

/**
 * Set function return type
 * @param item Item containing function data
 * @param type Function return type
 */
void func_set_rt(htab_item* item, token_e type);

/**
 * Function was already defined
 * @param item Item containing function data
 */
void func_set_def(htab_item* item);

#endif //IFJ17_COMPILER_SYMTABLE_H
