#pragma once

#include "wheel_ex.h"
#include <stdbool.h>
#include <stddef.h>
#include "StringSrc.h"

typedef struct String String;
typedef String* string;
typedef struct MutableString MutableString;
typedef MutableString* mutableString;

wheels string jcstr_new(void);

wheels string jcstr_newFrom(const char* const content);

wheels void jcstr_delete(string str);

wheels int jcstr_getStatus(const String* const str);

wheels size_t jcstr_getLength(const String* const str);

wheels const char* jcstr_statusToCharArray(const int status);


wheels string jcstr_appendWithInst(const String* const str1, const String* const str2);

wheels string jcstr_appendWithCh(const String* const str, const char* const content);

wheels string jcstr_reverse(const String* const str);

wheels int jcstr_split(const String* const str, char delimiter, string** outArray, size_t* count, bool removeEmptyEntries);

wheels string jcstr_trim(const String* const str);

wheels string jcstr_trimFront(const String* const str);

wheels string jcstr_trimBack(const String* const str);

wheels string jcstr_clone(const String* const str);

wheels int jcstr_charAt(const String* const str, size_t index, char* result);// oi

wheels int jcstr_indexOfFront(const String* const str, char ch, size_t* result);

wheels int jcstr_indexOfBack(const String* const str, char ch, size_t* result);

wheels int jcstr_indexOfFrontWithInst(const String* const str, const String* const sub, size_t* result);

wheels int jcstr_indexOfFrontWithCh(const String* const str, const char* const ch, size_t* result);

wheels string jcstr_subInLengthFrontWithInst(const String* const str, size_t start, size_t length);

wheels string jcstr_subInLengthBackWithInst(const String* const str, size_t end, size_t length);

wheels string jcstr_subInRangeWithInst(const String* const str, size_t start, size_t end);

wheels string jcstr_subInLengthFrontWithCh(const char* const ch, size_t start, size_t length);

wheels string jcstr_subInLengthBackWithCh(const char* const ch, size_t end, size_t length);

wheels string jcstr_subInRangeWithCh(const char* const ch, size_t start, size_t end);

wheels string jcstr_replaceFirstWithInst(const String* const str,const char* const target, const String* const replacement);

wheels string jcstr_replaceAllWithInst(const String* const str, const char* const target, const String* const replacement);

wheels string jcstr_replaceFirstWithCh(const String* const str, const char* const target, const char* const replacement);

wheels string jcstr_replaceAllWithCh(const String* const str, const char* const target, const char* const replacement);

wheels bool jcstr_startWithCh(const String* const str, const char* const prefix);

wheels bool jcstr_endWithCh(const String* const str, const char* const suffix);

wheels bool jcstr_startWithInst(const String* const str, const String* const prefix);

wheels bool jcstr_endWithInst(const String* const str, const String* const suffix);

wheels bool jcstr_isEmpty(const String* const str);

wheels bool jcstr_isBlank(const String* const str);

wheels bool jcstr_equalsWithInst(const String* const str1, const String* const str2);

wheels bool jcstr_equalsIgnoreCaseWithInst(const String* const str1, const String* const str2);

wheels bool jcstr_equalsWithCh(const String* const str, const char* const ch);

wheels bool jcstr_equalsIgnoreCaseWithCh(const String* const str, const char* const ch);

wheels bool jcstr_containsWithInst(const String* const str1, const String* const str2);

wheels bool jcstr_containsIgnoreCaseWithInst(const String* const str1, const String* const str2);

wheels bool jcstr_containsWithCh(const String* const str, const char* const ch);

wheels bool jcstr_containsIgnoreCaseWithCh(const String* const str, const char* const ch);

wheels int jcstr_compareWithInst(const String* const str1, const String* const str2, int* result);

wheels int jcstr_compareIgnoreCaseWithInst(const String* const str1, const String* const str2, int* result);

wheels int jcstr_compareWithCh(const String* const str, const char* const ch, int* result);

wheels int jcstr_compareIgnoreCaseWithCh(const String* const str, const char* const ch, int* result);


wheels char* jcstr_toCharArray(const String* const str);

wheels int jcstr_toInt(const String* const str, int* result);

wheels int jcstr_toLong(const String* const str, long* result);

wheels int jcstr_toLongLong(const String* const str, long long* result);

wheels int jcstr_toFloat(const String* const str, float* result);

wheels int jcstr_toDouble(const String* const str, double* result);

wheels string jcstr_toLowerCase(const String* const str);

wheels string jcstr_toUpperCase(const String* const str);

wheels mutableString jcstr_toMutableString(const String* const str);