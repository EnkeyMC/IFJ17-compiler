/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#ifndef IFJ17_COMPILER_BUFFER_H
#define IFJ17_COMPILER_BUFFER_H

#include <stddef.h>
#include <stdbool.h>

#define BUFFER_CHUNK 10

/**
 * Buffer object structure
 *
 * Buffer allocates more memory than needed, to avoid large amount
 * of reallocations. Buffer is reallocated by BUFFER_CHUNK only
 * when it runs out of memory.
 *
 * Buffer always holds a valid C string in arr. Buffer takes
 * care of appending '\0' at the end of the string.
 */
typedef struct {
    char* str;              /// Character array with dimension of buffer_size
    size_t len;             /// Valid string length in arr
    size_t buffer_size;     /// Allocated arr size
} Buffer;

/**
 * Allocate new buffer of given size, size has to be at least 1 for binary zero.
 * Size is automatically clamped to at least 1
 * @param size
 * @return new Buffer
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
 */
void buffer_append_c(Buffer* b, char c);

/**
 * Append string to buffer
 * @param b buffer
 * @param str string to append
 */
void buffer_append_str(Buffer* b, const char* str);

/**
 * Reallocates buffer to default size and sets length to 0
 * @param b buffer
 */
void buffer_clear(Buffer* b);

/**
 * Copies given string to buffer
 * @param b buffer
 * @param str string to copy
 */
void buffer_set_str(Buffer* b, const char* str);

/**
 * Debug print for buffer
 * @param b Buffer
 */
void buffer_debug(void* b);

#endif //IFJ17_COMPILER_BUFFER_H
