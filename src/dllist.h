#include <stdbool.h>

#ifndef IFJ17_COMPILER_LIST_H
#define IFJ17_COMPILER_LIST_H

#ifndef DEREF_DATA
// Macro for easier and cleaner data dereferencing
#define DEREF_DATA(expr, type) *((type *) (expr))
#endif // DEREF_DATA

/**
 * DLList item
 */
typedef struct dllist_item_t {
    void* data;  /// Generic item data
    struct dllist_item_t* next;  /// Next item in list
    struct dllist_item_t* prev;  /// Previous item in list
} DLListItem;

typedef void (*free_data_f) (void* data);

/**
 * Double Linked List object structure
 */
typedef struct dllist_t {
    DLListItem* first;  /// First list item
    DLListItem* last;  /// Last list item
    DLListItem* active;  /// Active item
    free_data_f free_data;  /// Function for data deallocation, can be NULL
} DLList;

/**
 * Initialize list
 * @param free_data funtion for data dealloction, can be NULL
 * @return new list, NULL on allocation error
 */
DLList* dllist_init(free_data_f free_data);

/**
 * Free list and free list data with given function free_data_f
 * @param list List
 * @param free_data_f callback function to free list data, can be NULL
 */
void dllist_free(DLList *l);

/**
 * Insert item on the first position in the list
 * @param l List
 * @param data generic data to insert
 * @return true on success, false otherwise
 */
bool dllist_insert_first(DLList *l, void *data);

/**
 * Insert item on the last position in the list
 * @param l List
 * @param data generic data to insert
 * @return true on success, false otherwise
 */
bool dllist_insert_last(DLList *l, void *data);

/**
 * Insert item after active item, no effect if list is not active
 * @param l List
 * @param data generic data to insert
 * @return true on success, false otherwise
 */
bool dllist_post_insert(DLList *l, void *data);

/**
 * Insert item before active item, no effect if list is not active
 * @param l List
 * @param data generic data to insert
 * @return true on success, false otherwise
 */
bool dllist_pre_insert(DLList *l, void *data);

/**
 * Is list empty
 * @param l List
 * @return true if empty, false otherwise
 */
bool dllist_empty(DLList *l);

/**
 * Is list active
 * @param l List
 * @return true if active, false otherwise
 */
bool dllist_active(DLList *l);

/**
 * Get data from active item, NULL if list is not active
 * @param l List
 * @return generic data, NULL if list is not active
 */
void* dllist_get_active(DLList *l);

/**
 * Get data from first item
 * @param l List
 * @return generic data, NULL if list is empty
 */
void* dllist_get_first(DLList *l);

/**
 * Get data from last item
 * @param l List
 * @return generic data, NULL if list is empty
 */
void* dllist_get_last(DLList *l);

/**
 * Delete first item from list and return it
 * @param l List
 * @return item data
 */
void* dllist_delete_first(DLList *l);

/**
 * Delete last item from list and return it
 * @param l List
 * @return item data
 */
void* dllist_delete_last(DLList *l);

/**
 * Delete active item from list and return it, activity will shift to next item
 * @param l List
 * @return item data
 */
void* dllist_delete_and_succ(DLList *l);

/**
 * Delete active item from list and return it, activity will shift to previous item
 * @param l List
 * @return item data
 */
void* dllist_delete_and_pred(DLList *l);

/**
 * Update value of active item, returns old value to free. No action if list is not active
 * @param l List
 * @param data new data to store in item
 * @return old value that may need deallocation, NULL if list is not active
 */
void* dllist_update(DLList *l, void* data);

/**
 * Set first item as active
 * @param l List
 */
void dllist_activate_first(DLList *l);

/**
 * Set last item as active
 * @param l List
 */
void dllist_activate_last(DLList *l);

/**
 * Set next item as active
 * @param l List
 */
void dllist_succ(DLList *l);

/**
 * Set previous item as active
 * @param l List
 */
void dllist_pred(DLList *l);

/**
 * List length
 * @param l List
 * @return length of the list
 */
int dllist_length(DLList *l);


#endif //IFJ17_COMPILER_LIST_H
