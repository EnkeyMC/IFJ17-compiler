#include <stdbool.h>

#ifndef IFJ17_COMPILER_LIST_H
#define IFJ17_COMPILER_LIST_H

/**
 * List item
 */
struct list_item_t {
    void* data;  /// Generic item data
    struct list_item_t* next;  /// Next item in list
};

/**
 * List object structure
 */
typedef struct list_t {
    struct list_item_t* first;  /// First list item
    struct list_item_t* last;  /// Last list item
    struct list_item_t* active;  /// Active item
} List;

/**
 * Initialize empty list
 * @return new list, NULL on allocation error
 */
List* list_init();

/**
 * Free list and free list data with given function free_data_f
 * @param list List
 * @param free_data_f callback function to free list data, can be NULL
 */
void list_free(List* list, void (*free_data_f) (void* data));

/**
 * Insert item on the first position in the list
 * @param l List
 * @param data generic data to insert
 * @return true on success, false otherwise
 */
bool list_insert_first(List* l, void* data);

/**
 * Insert item on the last position in the list
 * @param l List
 * @param data generic data to insert
 * @return true on success, false otherwise
 */
bool list_insert_last(List* l, void* data);

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
 * Get data from last item
 * @param l List
 * @return generic data, NULL if list is empty
 */
void* list_get_last(List* l);

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
