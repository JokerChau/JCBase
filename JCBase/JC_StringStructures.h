#pragma once

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
	GROWTH = 2,
}strSizes;

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