/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "utils.h"
#include "buffer.h"
#include "memory_manager.h"

char* generate_uid() {
	static unsigned int seed = 1;
	Buffer* buffer = buffer_init(5);

	buffer_append_str(buffer, "ID");

	unsigned int num = seed;
	while (num) {
		buffer_append_c(buffer, (char) (num % 10 + '0'));
		num /= 10;
	}

	seed++;

	char* out = (char*) mm_malloc(sizeof(char) * (strlen(buffer->str) + 1));

	strcpy(out, buffer->str);
	buffer_free(buffer);
	return out;
}

char* concat(const char* str1, const char* str2) {
	assert(str1 != NULL);
	assert(str2 != NULL);

	char* result = (char*) mm_malloc(sizeof(char) * (strlen(str1) + strlen(str2) + 1));

	strcpy(result, str1);
	strcat(result, str2);

	return result;
}
