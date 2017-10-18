#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "buffer.h"

static int buffer_realloc(Buffer* b, size_t size) {
	b->arr = (char*) realloc(b->arr, size);
	if (b->arr == NULL) {
		b->buffer_size = 0;
		b->len = 0;
		return -1;
	}

	if (size < b->buffer_size)
		b->len = 0;

	b->buffer_size = size;

	return 0;
}

Buffer* buffer_init(size_t size) {
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
}

void buffer_free(Buffer* b) {
	assert(b != NULL);
	if (b->arr != NULL)
		free(b->arr);
	free(b);
}

int buffer_append_c(Buffer* b, char c) {
	assert(b != NULL);

	if (b->len + 1 == b->buffer_size) {
		if (!buffer_realloc(b, b->buffer_size + BUFFER_CHUNK))
			return -1;
	}

	b->arr[b->len] = c;
	b->arr[b->len+1] = '\0';
	return 0;
}

int buffer_append_str(Buffer* b, const char* str) {
	assert(b != NULL);

	if (b->len + strlen(str) + 1 > b->buffer_size) {
		if (!buffer_realloc(b, b->len + strlen(str)))
			return -1;
	}

	strcat(b->arr, str);
	return 0;
}

int buffer_clear(Buffer* b) {
	assert(b != NULL);

	if (!buffer_realloc(b, BUFFER_CHUNK))
		return -1;

	b->len = 0;
	b->arr = 0;

	return 0;
}

int buffer_set_str(Buffer* b, const char* str) {
	assert(b != NULL);

	if (strlen(str) + 1 > b->buffer_size)
		if (!buffer_realloc(b, strlen(str) + 1))
			return -1;

	strcpy(b->arr, str);
	return 0;
}
