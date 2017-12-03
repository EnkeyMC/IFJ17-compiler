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

#define BUFFER_INIT_SIZE 42
#define HTAB_INIT_SIZE 67

/**
 * Function hash table item
 */
typedef struct htab_function_item_t {
	token_e ret_type;	/// Return type
	unsigned params_num;	/// Number of parameters
	Buffer* param_types;	/// String containing paramater types
	Buffer* param_names;	/// String containing paramater names
	bool defined;	/// Was already defined?
} htab_function_item;

typedef struct htab_variable_item_t {
	token_e type;	/// Identifier type
} htab_variable_item;

/**
 * Identifier hash table item
 */
typedef struct htab_item_t {
	char *key;	/// Name of identifier
	struct htab_item_t * next;	/// Pointer to next item in the list
	union {
		htab_function_item* function;
		htab_variable_item* variable;
	};
} htab_item;

/**
 * Hash Table structure
 */
typedef struct hash_table {
	size_t bucket_count;	/// Number of buckets contained in the hash table
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
 * @param htab Pointer to hash table
 */
void htab_free(void* htab);
void htab_func_free(HashTable *htab);

/**
 * Find item
 * @param htab Pointer to hash table
 * @param key String identifying an item
 * @return Pointer to item or NULL if the item does not exist
 */
htab_item * htab_find(HashTable *htab, const char* key);

/**
 * Remove bucket containing given key
 * @param htab Pointer to hash table
 * @param key String identifying an item
 * @return true on seccuss or false if the item does not exist
 */
bool htab_remove(HashTable *htab, const char *key);
bool htab_func_remove(HashTable *htab, const char *key);

/**
 * Find existing bucket or add new one if it does not exist
 * @param htab Pointer to hash table
 * @param key String identifying an item
 * @return Pointer to item or NULL if allocation of memory for new item fails
 */
htab_item* htab_lookup(HashTable *htab, const char* key);
htab_item* htab_func_lookup(HashTable *htab, const char* key);

/**
 * For each entry in the hash table call function 'func'
 * @param htab Pointer to hash table
 * @param func Pointer to function which takes pointer to hash table item as argument
 */
void htab_foreach(HashTable *htab, void (*func)(htab_item *item));

/**
 * Print contents of hash table item. Ment to be used with htab_foreach primarily
 * @param item Pointer to hash table item
 */
void variable_item_debug(htab_item *item);
void function_item_debug(htab_item *item);

// --------------------------------------------------------------------
// FUNCTIONS TO MODIFY/ACCESS HASH TABLE ITEMS (and their 'attributes')
// --------------------------------------------------------------------

/**
 * Add parameter type to function record in hash table
 * @param item Pointer to hash table item (that stores function data)
 * @param type parameter data type
 */
void func_add_param(htab_item* item, token_e type);

/**
 * Get type of function parameter from given index
 * @param item Pointer to hash table item (that stores function data)
 * @param idx Parameter index - INDEXING STARTS AT 1 !!!
 * @return Parameter data type (e.g. TOKEN_KW_STRING, TOKEN_KW_INTEGER,...) on success, END_OF_TERMINALS otherwise
 */
token_e func_get_param(htab_item* item, unsigned idx);

/**
 * Auxiliary function for comparing function definition and declaration
 * @param item function data
 * @return index of current param
 */
unsigned func_get_param_idx(htab_item* item);

/**
 * Store parameter name in function atribute
 * @param item Item with function data
 * @param name name of the parameter
 */
void func_store_param_name(htab_item* item, const char* name);

/**
 * Get parameter name
 * @param item Item with function data
 * @param idx Parameter index - INDEXING STARTS AT 1 !!!
 * @return Copy of parameter name on success
 */
char* func_get_param_name(htab_item* item, unsigned idx);

/**
 * Get number of function parameters
 * @param item Item with function data
 * @return number of parameters
 */
unsigned int func_get_params_num(htab_item* item);

/**
 * Set function return type
 * @param item Item containing function data
 * @param type Function return type
 */
void func_set_ret_type(htab_item* item, token_e type);

/**
 * Get function return type
 * @param item Item containing function data
 * @return function return type
 */
token_e func_get_ret_type(htab_item* item);

/**
 * Function definition provided
 * @param item Item containing function data
 */
void func_set_defined(htab_item* item);

/**
 * Function was already defined
 * @param item Item containing function data
 * @return true if function was already defined
 */
bool func_get_defined(htab_item* item);

/**
 * Check whether all declared functions have been defined
 * @param htab hash table storing function records
 * @return true if every function was correctly defined, false otherwise
 */
bool func_check_all_defined(HashTable *htab);

/**
 * Get variable type
 * @param item
 */
token_e var_get_type(htab_item* item);

/**
 * Set variable type
 * @param item
 * @param type
 */
void var_set_type(htab_item* item, token_e type);

#endif //IFJ17_COMPILER_SYMTABLE_H
