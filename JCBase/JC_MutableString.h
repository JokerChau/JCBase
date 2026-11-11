#pragma once

#include "wheel_ex.h"
#include "JC_MutableStringSrc.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct JC_MutableString JC_MutableString;
typedef JC_MutableString* jc_MutableString;
typedef struct JC_String JC_String;
typedef JC_String* jc_String;

wheels jc_MutableString jcms_new(void);

wheels jc_MutableString jcms_newFrom(const char* const content);

wheels void jcms_delete(jc_MutableString ms);

wheels int jcms_getStatus(const JC_MutableString* const ms);

wheels size_t jcms_getLength(const JC_MutableString* const ms);

wheels const char* jcms_viewBuffer(const JC_MutableString* const ms);

wheels int jcms_setContent(const jc_MutableString ms, const char* const content);

wheels int jcms_changeAt(const jc_MutableString ms, size_t index, char ch);

wheels const char* jcms_statusToCharArray(const int status);

wheels int jcms_appendWithInst(const jc_MutableString ms, const JC_MutableString* const src);

wheels int jcms_appendWithCh(const jc_MutableString ms, const char* const ch);

wheels int jcms_reverse(jc_MutableString ms);

wheels int jcms_split(const JC_MutableString* const ms, char delimiter,
	jc_MutableString** outArray, size_t* count,
	bool removeEmptyEntries);

wheels int jcms_trim(const jc_MutableString ms);

wheels int jcms_trimFront(const jc_MutableString ms);

wheels int jcms_trimBack(const jc_MutableString ms);

wheels jc_MutableString jcms_clone(const JC_MutableString* const ms);

wheels int jcms_charAt(const JC_MutableString* const ms, size_t index, char* result);

wheels int jcms_indexOfFront(const JC_MutableString* const ms, char ch, size_t* result);

wheels int jcms_indexOfBack(const JC_MutableString* const ms, char ch, size_t* result);

wheels int jcms_indexOfFrontWithInst(const JC_MutableString* const ms, const JC_MutableString* const sub, size_t* result);

wheels int jcms_indexOfFrontWithCh(const JC_MutableString* const ms, const char* const ch, size_t* result);

wheels jc_MutableString jcms_subInLengthFrontWithInst(const JC_MutableString* const ms, size_t start, size_t length);

wheels jc_MutableString jcms_subInLengthBackWithInst(const JC_MutableString* const ms, size_t end, size_t length);

wheels jc_MutableString jcms_subInRangeWithInst(const JC_MutableString* const ms, size_t start, size_t end);

wheels jc_MutableString jcms_subInLengthFrontWithCh(const char* const ch, size_t start, size_t length);

wheels jc_MutableString jcms_subInLengthBackWithCh(const char* const ch, size_t end, size_t length);

wheels jc_MutableString jcms_subInRangeWithCh(const char* const ch, size_t start, size_t end);

wheels int jcms_replaceFirstWithInst(JC_MutableString* ms, const char* const target, const JC_MutableString* const replacement);

wheels int jcms_replaceFirstWithCh(JC_MutableString* ms, const char* const target, const char* const replacement);

wheels int jcms_replaceAllWithInst(JC_MutableString* ms, const char* const target, const JC_MutableString* const replacement);

wheels int jcms_replaceAllWithCh(JC_MutableString* ms, const char* const target, const char* const replacement);

wheels bool jcms_startWithCh(const JC_MutableString* const ms, const char* const prefix);

wheels bool jcms_startWithInst(const JC_MutableString* const ms, const JC_MutableString* const prefix);

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