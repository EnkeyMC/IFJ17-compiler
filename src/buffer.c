/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "buffer.h"
#include "debug.h"
#include "memory_manager.h"


static void buffer_realloc(Buffer* b, size_t size) {
	if (size == b->buffer_size)
		return;

	b->str = (char*) mm_realloc(b->str, size);

	if (size < b->buffer_size) {
		b->len = 0;
		if (size != 0)
			b->str[0] = '\0';
	}

	b->buffer_size = size;
}

Buffer* buffer_init(size_t size) {
	if (size < 1)
		size = 1;

	Buffer* b = (Buffer*) mm_malloc(sizeof(Buffer));

	b->buffer_size = size;
	b->len = 0;
	b->str = (char*) mm_malloc(sizeof(char) * b->buffer_size);

	b->str[0] = '\0';

	return b;
}

void buffer_free(Buffer* b) {
	assert(b != NULL);
	if (b->str != NULL)
		mm_free(b->str);
	mm_free(b);
}

void buffer_append_c(Buffer* b, char c) {
	assert(b != NULL);

	if (b->len + 1 >= b->buffer_size) {
		buffer_realloc(b, b->buffer_size + BUFFER_CHUNK);
	}
  
	b->str[b->len] = c;
	b->str[b->len+1] = '\0';
	b->len++;
}

void buffer_append_str(Buffer* b, const char* str) {
	assert(b != NULL);

	size_t str_len = strlen(str);

	if (b->len + str_len + 1 > b->buffer_size) {
		buffer_realloc(b, b->len + strlen(str) + 1);
	}

	strcat(b->str, str);
	b->len += str_len;
}

void buffer_clear(Buffer* b) {
	assert(b != NULL);

	buffer_realloc(b, BUFFER_CHUNK);

	b->str[0] = '\0';
	b->len = 0;
	b->buffer_size = BUFFER_CHUNK;
}

void buffer_set_str(Buffer* b, const char* str) {
	assert(b != NULL);

	size_t str_len = strlen(str);

	if (str_len + 1 > b->buffer_size)
		buffer_realloc(b, strlen(str) + 1);

	strcpy(b->str, str);
	b->len = str_len;
}

void buffer_debug(void* b) {
	Buffer* buffer = (Buffer*) b;
	debug("Buffer@%p: {", buffer);

	if (buffer != NULL) {
		debug(".len = %lu, .buffer_size = %lu, .str = %s", buffer->len, buffer->buffer_size, buffer->str);
	}

	debugs("}");
}
