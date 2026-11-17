#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

/*
* Design of String:
* 1. API rules of name: jcstr_<function name>/jcms_<function name>, the crazy branch should use name: jccstr_<function name>
* 2. Part String:
*
*	typedef struct JC_String {
*		char* buffer;
*		int status;
*		size_t cachedLen;
*	}JC_String;
*
*	typedef JC_String* jc_String;
* 
* 3. Part MutableString:
*	
*	typedef struct JC_MutableString {
* 		char* buffer;
* 		int status;
* 		size_t len;
* 		size_t cap;
* 	}JC_MutableString;
* 
*	typedef JC_MutableString* jc_MutableString;
* 
* 4. Part crazy String:
* 
*	typedef struct JC_crazyString {
* 		char* buffer;
* 		size_t len;
* 		size_t cap;
* 	}JC_crazyString;
* 
*	typedef JC_crazyString* jc_crazyString;
*
* 5. Rules of naming status:
*
*		Take String as an example, the status of the String is defined as follows:
*		JC_STR__NULL = -2,					// NULL String实例
*		Take crazy String as an example, the status of the crazy String is defined as follows:
*		JC_CSTR__NULL = -2,					// NULL crazy String实例
*
*       JC_StringStatus as the name of JC_String
*
* 6. About file name:
*		Take String as an example, the file name is JC_String.h, the source file name is JC_String.c, and the header file for the status definition is JC_StringSrc.h.
*/

typedef struct JC_String {
	char* buffer;
	int status;
	size_t cachedLen;
}JC_String;

typedef enum {
	INITIAL_STRING_SIZE = 200,
	INITIAL_MS_SIZE = 200,
	INITIAL_C_STRING_SIZE = 200,
	GROWTH = 2,
}strSizes;

typedef enum JC_SpaceMode {
	JC_SPACE_LOCALE = 0,
	JC_SPACE_ASCII,
} JC_SpaceMode;

typedef struct JC_MutableString {
	char* buffer;
	int status;
	size_t len;
	size_t cap;
}JC_MutableString;

typedef struct JC_crazyString {
	char* buffer;
	size_t len;
	size_t cap;
}JC_crazyString;

// mode: 0 = 跟随 locale（标准 isspace）, 1 = 固定 ASCII 空白
// 未知 mode 行为和 0 一致，避免“意外把非空白当空白吞掉”
static inline bool isSpaceInternal(unsigned char c, JC_SpaceMode mode) {
	switch (mode) {
	case JC_SPACE_ASCII:
		return c == ' ' || c == '\t' || c == '\n' ||
			c == '\r' || c == '\v' || c == '\f';
	case JC_SPACE_LOCALE:
	default:
		return isspace((int)c) != 0;
	}
}

static inline int toLowerInternal(unsigned char c, JC_SpaceMode mode) {
	switch (mode) {
	case JC_SPACE_ASCII:
		return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
	default:
		return tolower((int)c);
	}
}

static inline int toUpperInternal(unsigned char c, JC_SpaceMode mode) {
	switch (mode) {
	case JC_SPACE_ASCII:
		return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
	default:
		return toupper((int)c);
	}
}

// 忽略大小写的子串查找——needle 可为空串（空串是任何串的子串）
static inline bool containsIgnoreCase(const char* haystack, const char* needle, JC_SpaceMode mode) {
	size_t hayLen = strlen(haystack);
	size_t needleLen = strlen(needle);
	if (needleLen > hayLen) return false;

	for (size_t i = 0; i + needleLen <= hayLen; i++) {
		bool match = true;
		for (size_t j = 0; j < needleLen; j++) {
			if (toLowerInternal((unsigned char)haystack[i + j], mode) !=
				toLowerInternal((unsigned char)needle[j], mode)) {
				match = false;
				break;
			}
		}
		if (match) return true;
	}
	return false;
}

// 忽略大小写的字符串比较——返回 <0 / 0 / >0
static inline int compareIgnoreCase(const char* a, const char* b, JC_SpaceMode mode) {
	while (*a && *b) {
		int ca = toLowerInternal((unsigned char)*a, mode);
		int cb = toLowerInternal((unsigned char)*b, mode);
		if (ca != cb) return ca - cb;
		a++;
		b++;
	}
	return (unsigned char)*a - (unsigned char)*b;
}