#include "JC_STDIO.h"
#include "JC_StringStructures.h"
#include "JC_StringSrc.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

jc_String nextLine() {
	jc_String str = (jc_String)malloc(sizeof(JC_String));
	if (!str) {
		return NULL;
	}

	str->buffer = (char*)calloc(INITIAL_STRING_SIZE, sizeof(char));
	if (!str->buffer) {
		str->status = JC_STR__NULL_BUFFER;
		str->buffer = NULL;
		str->cachedLen = 0;
		return str;
	}

	size_t capacity = INITIAL_STRING_SIZE;
	size_t len = 0;
	int ch;

	while ((ch = getchar()) != EOF && ch != '\n') {
		if (len + 1 >= capacity) {
			if (capacity > SIZE_MAX / GROWTH) {
				free(str->buffer);
				str->buffer = NULL;
				str->status = JC_STR__REALLOCATION_FAILED;
				str->cachedLen = 0;
				return str;
			}
			size_t newCapacity = capacity * GROWTH;
			char* newBuffer = (char*)realloc(str->buffer, newCapacity);
			if (!newBuffer) {
				free(str->buffer);
				str->buffer = NULL;
				str->status = JC_STR__REALLOCATION_FAILED;
				str->cachedLen = 0;
				return str;
			}
			str->buffer = newBuffer;
			capacity = newCapacity;
		}
		str->buffer[len++] = (char)ch;
	}

	str->buffer[len] = '\0';

	str->status = JC_STR__AVAILABLE;
	str->cachedLen = len;

	return str;
}