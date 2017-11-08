#include <stdbool.h>

#ifndef IFJ17_COMPILER_LIST_H
#define IFJ17_COMPILER_LIST_H

/**
 * List item
 */
typedef struct list_item_t {
    void* data;  /// Generic item data
    struct list_item_t* next;  /// Next item in list
} ListItem;

typedef void (*free_data_f) (void* data);

/**
 * List object structure
 */
typedef struct list_t {
    ListItem* first;  /// First list item
    ListItem* active;  /// Active item
    free_data_f free_data;  /// Function for data deallocation, can be NULL
} List;

/**
 * Initialize list
 * @param free_data funtion for data dealloction, can be NULL
 * @return new list, NULL on allocation error
 */
List* list_init(free_data_f free_data);

/**
 * Free list and free list data with given function free_data_f
 * @param list List
 * @param free_data_f callback function to free list data, can be NULL
 */
void list_free(List* l);

/**
 * Insert item on the first position in the list
 * @param l List
 * @param data generic data to insert
 * @return true on success, false otherwise
 */
bool list_insert_first(List* l, void* data);

/**
 * Insert item after active item, no effect if list is not active
 * @param l List
 * @param data generic data to insert
 * @return true on success, false otherwise
 */
bool list_post_insert(List* l, void* data);

/**
 * Is list empty
 * @param l List
 * @return true if empty, false otherwise
 */
bool list_empty(List* l);

/**
 * Is list active
 * @param l List
 * @return true if active, false otherwise
 */
bool list_active(List* l);

/**
 * Get data from active item, NULL if list is not active
 * @param l List
 * @return generic data, NULL if list is not active
 */
void* list_get_active(List* l);

/**
 * Get data from first item
 * @param l List
 * @return generic data, NULL if list is empty
 */
void* list_get_first(List* l);

/**
 * Delete first item from list and return it
 * @param l List
 * @return item data
 */
void* list_delete_frist(List* l);

/**
 * Delete active item from list and return it, activity will shift to next item
 * @param l List
 * @return item data
 */
void* list_delete_active(List* l);

/**
 * Set first item as active
 * @param l List
 */
void list_activate(List* l);

/**
 * Set next item as active
 * @param l List
 */
void list_succ(List* l);

/**
 * List length
 * @param l List
 * @return length of the list
 */
int list_length(List* l);


#endif //IFJ17_COMPILER_LIST_H
