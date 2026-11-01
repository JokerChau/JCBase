#pragma once

#include "wheel_ex.h"
#include "MutableStringSrc.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct MutableString MutableString;
typedef MutableString* mutableString;
typedef struct String String;
typedef String* string;

wheels mutableString jcms_new(void);

wheels mutableString jcms_newFrom(const char* const content);

wheels void jcms_delete(mutableString ms);

wheels int jcms_getStatus(const MutableString* const ms);

wheels size_t jcms_getLength(const MutableString* const ms);

wheels const char* jcms_viewBuffer(const MutableString* const ms);

wheels int jcms_setContent(const mutableString ms, const char* const content);

wheels int jcms_changeAt(const mutableString ms, size_t index, char ch);

wheels const char* jcms_statusToCharArray(const int status);

wheels int jcms_appendWithInst(const mutableString ms, const MutableString* const src);

wheels int jcms_appendWithCh(const mutableString ms, const char* const ch);

wheels int jcms_reverse(mutableString ms);

wheels int jcms_split(const MutableString* const ms, char delimiter,
	mutableString** outArray, size_t* count,
	bool removeEmptyEntries);

wheels int jcms_trim(const mutableString ms);

wheels int jcms_trimFront(const mutableString ms);

wheels int jcms_trimBack(const mutableString ms);

wheels mutableString jcms_clone(const MutableString* const ms);

wheels int jcms_charAt(const MutableString* const ms, size_t index, char* result);

wheels int jcms_indexOfFront(const MutableString* const ms, char ch, size_t* result);

wheels int jcms_indexOfBack(const MutableString* const ms, char ch, size_t* result);

wheels int jcms_indexOfFrontWithInst(const MutableString* const ms, const MutableString* const sub, size_t* result);

wheels int jcms_indexOfFrontWithCh(const MutableString* const ms, const char* const ch, size_t* result);

wheels mutableString jcms_subInLengthFrontWithInst(const MutableString* const ms, size_t start, size_t length);

wheels stringBuilder subSbInLengthBack(const StringBuilder* const str, size_t end, size_t length);

wheels stringBuilder subSbInRange(const StringBuilder* const str, size_t start, size_t end);

wheels mutableString jcms_subInLengthFrontWithCh(const char* const ch, size_t start, size_t length);

wheels stringBuilder subSbInLengthBackFrom(const char* const ch, size_t end, size_t length);

wheels stringBuilder subSbInRangeFrom(const char* const ch, size_t start, size_t end);

wheels int replaceSbFirst(StringBuilder* str, const char* const target, const StringBuilder* const replacement);

wheels int replaceSbFirstFrom(StringBuilder* str, const char* const target, const char* const replacement);

wheels int replaceSbAll(StringBuilder* str, const char* const target, const StringBuilder* const replacement);

wheels int replaceSbAllFrom(StringBuilder* str, const char* const target, const char* const replacement);

wheels bool sbStartWith(const StringBuilder* const str, const char* const prefix);

wheels bool sbEndWith(const StringBuilder* const str, const char* const suffix);

wheels bool sbStartWithString(const StringBuilder* const str, const StringBuilder* const prefix);

wheels bool sbEndWithString(const StringBuilder* const str, const StringBuilder* const suffix);

wheels bool sbIsEmpty(const StringBuilder* const str);

wheels bool sbIsBlank(const StringBuilder* const str);

wheels bool equalsSb(const StringBuilder* const str1, const StringBuilder* const str2);

wheels bool equalsSbFrom(const StringBuilder* const str, const char* const ch);

wheels bool containsSb(const StringBuilder* const str1, const StringBuilder* const str2);

wheels bool containsSbFrom(const StringBuilder* const str, const char* const ch);

wheels char* sbToArray(const StringBuilder* const str);

wheels int sbToInt(const StringBuilder* const str, int* result);

wheels int sbToLong(const StringBuilder* const str, long* result);

wheels int sbToLongLong(const StringBuilder* const str, long long* result);

wheels int sbToFloat(const StringBuilder* const str, float* result);

wheels int sbToDouble(const StringBuilder* const str, double* result);

wheels int sbToLowerCase(StringBuilder* str);

wheels int sbToUpperCase(StringBuilder* str);

wheels string sbToString(const StringBuilder* const str);