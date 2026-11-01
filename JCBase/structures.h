#pragma once

typedef struct String {
	char* buffer;
	int status;
	size_t cachedLen;
}String;

typedef enum {
	INITIAL_STRING_SIZE = 200,
	INITIAL_MS_SIZE = 200,
}strSizes;

typedef struct MutableString {
	char* buffer;
	int status;
	size_t len;
	size_t cap;
}MutableString;