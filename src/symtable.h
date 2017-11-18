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

/**
 * Hash table item type
 */
typedef struct htab_item {
	char *key;
	token_e type;  /// Variable type or function return type
	struct htab_item * next;	/// Pointer to next item in the list

	// TODO
	// add some additional data

} htab_item;

/**
 * Hash table type
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
void htab_free(HashTable *htab_ptr);

/**
 * Find item
 * @param htab_ptr Pointer to hash table
 * @param key String identifying an item
 * @return Pointer to item or NULL if the item does not exist
 */
htab_item * htab_find(HashTable *htab_ptr, const char *key);

/**
 * Find existing bucket or add new one if it does not exist
 * @param htab_ptr Pointer to hash table
 * @param key String identifying an item
 * @return Pointer to item or NULL if allocation of memory for new item fails
 */
htab_item * htab_lookup(HashTable *htab_ptr, const char *key);

/**
 * Remove given bucket
 * @param htab_ptr Pointer to hash table
 * @param key String identifying an item
 * @return true on seccuss or false if the item does not exist
 */
bool htab_remove(HashTable *htab_ptr, const char *key);

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
 * Print contents of an item to stdout. Use with htab_foreach in first place.
 * @param item_ptr Pointer to hash table item
 */
void print_item(htab_item *item_ptr);

#endif //IFJ17_COMPILER_SYMTABLE_H
