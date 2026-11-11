#pragma once

#include "wheel_ex.h"
#include "JC_MutableStringSrc.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct JC_MutableString JC_MutableString;
typedef JC_MutableString* jc_MutableString;
typedef struct JC_String JC_String;
typedef JC_String* jc_String;

typedef enum JC_SpaceMode JC_SpaceMode;

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

wheels int jcms_trim(const jc_MutableString ms, JC_SpaceMode mode);

wheels int jcms_trimFront(const jc_MutableString ms, JC_SpaceMode mode);

wheels int jcms_trimBack(const jc_MutableString ms, JC_SpaceMode mode);

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

wheels bool jcms_endWithCh(const JC_MutableString* const ms, const char* const suffix);

wheels bool jcms_endWithInst(const JC_MutableString* const ms, const JC_MutableString* const suffix);

wheels bool jcms_isEmpty(const JC_MutableString* const ms);

wheels bool jcms_isBlank(const JC_MutableString* const ms, JC_SpaceMode mode);

wheels bool jcms_equalsWithInst(const JC_MutableString* const ms, const JC_MutableString* const obj);

wheels bool jcms_equalsIgnoreCaseWithInst(const JC_MutableString* const ms, const JC_MutableString* const obj, JC_SpaceMode mode);

wheels bool jcms_equalsWithCh(const JC_MutableString* const ms, const char* const obj);

wheels bool jcms_equalsIgnoreCaseWithCh(const JC_MutableString* const ms, const char* const obj, JC_SpaceMode mode);

wheels bool jcms_containsWithInst(const JC_MutableString* const ms, const JC_MutableString* const obj);

wheels bool jcms_containsIgnoreCaseWithInst(const JC_MutableString* const ms, const JC_MutableString* const obj, JC_SpaceMode mode);

wheels bool jcms_containsWithCh(const JC_MutableString* const ms, const char* const obj);

wheels bool jcms_containsIgnoreCaseWithCh(const JC_MutableString* const ms, const char* const obj, JC_SpaceMode mode);

wheels int jcms_compareWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2, int* result);

wheels int jcms_compareIgnoreCaseWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2, int* result, JC_SpaceMode mode);

wheels int jcms_compareWithCh(const JC_MutableString* const ms, const char* const ch, int* result);

wheels int jcms_compareIgnoreCaseWithCh(const JC_MutableString* const ms, const char* const ch, int* result, JC_SpaceMode mode);

wheels char* jcms_toCharArray(const JC_MutableString* const ms);

wheels int jcms_toInt(const JC_MutableString* const ms, int* result, JC_SpaceMode mode);

wheels int jcms_toLong(const JC_MutableString* const ms, long* result, JC_SpaceMode mode);

wheels int jcms_toLongLong(const JC_MutableString* const ms, long long* result, JC_SpaceMode mode);

wheels int jcms_toFloat(const JC_MutableString* const ms, float* result, JC_SpaceMode mode);

wheels int jcms_toDouble(const JC_MutableString* const ms, double* result, JC_SpaceMode mode);

wheels int jcms_toLowerCase(JC_MutableString* ms, JC_SpaceMode mode);

wheels int jcms_toUpperCase(JC_MutableString* ms, JC_SpaceMode mode);

wheels jc_String jcms_toString(const JC_MutableString* const ms);