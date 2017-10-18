#ifndef IFJ17_COMPILER_BUFFER_H
#define IFJ17_COMPILER_BUFFER_H

#include <stddef.h>

#define BUFFER_CHUNK 10

/**
 * Buffer object structure
 *
 * Buffer allocates more memory than needed, to avoid large amount
 * of reallocations. Buffer is reallocated by BUFFER_CHUNK only
 * when it runs out of memory.
 */
typedef struct {
    char* arr;              /// Character array with dimension of buffer_size
    size_t len;             /// Valid string length in arr
    size_t buffer_size;     /// Allocated arr size
} Buffer;

/**
 * Allocate new buffer of given size
 * @param size
 * @return new Buffer, NULL on allocation error
 */
Buffer* buffer_init(size_t size);

/**
 * Deallocates buffer memory
 * @param b buffer
 */
void buffer_free(Buffer* b);

/**
 * Append one character to buffer
 * @param b buffer
 * @param c character to append
 * @return 0 on success
 */
int buffer_append_c(Buffer* b, char c);

/**
 * Append string to buffer
 * @param b buffer
 * @param str string to append
 * @return 0 on success
 */
int buffer_append_str(Buffer* b, const char* str);

/**
 * Reallocates buffer to default size and sets length to 0
 * @param b buffer
 * @return 0 on success
 */
int buffer_clear(Buffer* b);

/**
 * Copies given string to buffer
 * @param b buffer
 * @param str string to copy
 * @return 0 on success
 */
int buffer_set_str(Buffer* b, const char* str);

#endif //IFJ17_COMPILER_BUFFER_H
