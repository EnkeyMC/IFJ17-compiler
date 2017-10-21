#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "buffer.h"

static bool buffer_realloc(Buffer* b, size_t size) {
	if (size == b->buffer_size)
		return true;

	b->arr = (char*) realloc(b->arr, size);
	if (b->arr == NULL) {
		b->buffer_size = 0;
		b->len = 0;
		return false;
	}

	if (size < b->buffer_size)
		b->len = 0;

	b->buffer_size = size;

	return true;
}

Buffer* buffer_init(size_t size) {
	if (size < 1)
		size = 1;

	Buffer* b = (Buffer*) malloc(sizeof(Buffer));
	if (b == NULL)
		return NULL;

	b->buffer_size = size;
	b->len = 0;
	b->arr = (char*) malloc(sizeof(char) * b->buffer_size);

	if (b->arr == NULL) {
		free(b);
		return NULL;
	}

	b->arr[0] = '\0';

	return b;
}

void buffer_free(Buffer* b) {
	assert(b != NULL);
	if (b->arr != NULL)
		free(b->arr);
	free(b);
}

bool buffer_append_c(Buffer* b, char c) {
	assert(b != NULL);

	if (b->len + 1 >= b->buffer_size) {
		if (!buffer_realloc(b, b->buffer_size + BUFFER_CHUNK))
			return false;
	}

	b->arr[b->len] = c;
	b->arr[b->len+1] = '\0';
	b->len++;
	return true;
}

bool buffer_append_str(Buffer* b, const char* str) {
	assert(b != NULL);

	size_t str_len = strlen(str);

	if (b->len + str_len + 1 > b->buffer_size) {
		if (!buffer_realloc(b, b->len + strlen(str) + 1)) {
			return false;
		}
	}

	strcat(b->arr, str);
	b->len += str_len;
	return true;
}

bool buffer_clear(Buffer* b) {
	assert(b != NULL);

	if (!buffer_realloc(b, BUFFER_CHUNK))
		return false;

	b->len = 0;
	b->buffer_size = BUFFER_CHUNK;

	return true;
}

bool buffer_set_str(Buffer* b, const char* str) {
	assert(b != NULL);

	size_t str_len = strlen(str);

	if (str_len + 1 > b->buffer_size)
		if (!buffer_realloc(b, strlen(str) + 1))
			return false;

	strcpy(b->arr, str);
	b->len = str_len;
	return true;
}
