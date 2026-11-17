#pragma once

#include "wheel_ex.h"
#include <stdbool.h>
#include <stddef.h>
#include "JC_StringSrc.h"

typedef struct JC_String JC_String;
typedef JC_String* jc_String;
typedef struct JC_MutableString JC_MutableString;
typedef JC_MutableString* jc_MutableString;

typedef enum JC_SpaceMode JC_SpaceMode;

wheels jc_String jcstr_newFrom(const char* const content);

wheels void jcstr_delete(jc_String str);

wheels int jcstr_getStatus(const JC_String* const str);

wheels size_t jcstr_getLength(const JC_String* const str);

wheels const char* jcstr_statusToCharArray(const int status);


wheels jc_String jcstr_appendWithInst(const JC_String* const str1, const JC_String* const str2);

wheels jc_String jcstr_appendWithCh(const JC_String* const str, const char* const content);

wheels jc_String jcstr_reverse(const JC_String* const str);

wheels int jcstr_split(const JC_String* const str, char delimiter, jc_String** outArray, size_t* count, bool removeEmptyEntries);

wheels jc_String jcstr_trim(const JC_String* const str, JC_SpaceMode mode);

wheels jc_String jcstr_trimFront(const JC_String* const str, JC_SpaceMode mode);

wheels jc_String jcstr_trimBack(const JC_String* const str, JC_SpaceMode mode);

wheels jc_String jcstr_clone(const JC_String* const str);

wheels int jcstr_charAt(const JC_String* const str, size_t index, char* result);// oi

wheels int jcstr_indexOfFront(const JC_String* const str, char ch, size_t* result);

wheels int jcstr_indexOfBack(const JC_String* const str, char ch, size_t* result);

wheels int jcstr_indexOfFrontWithInst(const JC_String* const str, const JC_String* const sub, size_t* result);

wheels int jcstr_indexOfFrontWithCh(const JC_String* const str, const char* const ch, size_t* result);

wheels jc_String jcstr_subInLengthFrontWithInst(const JC_String* const str, size_t start, size_t length);

wheels jc_String jcstr_subInLengthBackWithInst(const JC_String* const str, size_t end, size_t length);

wheels jc_String jcstr_subInRangeWithInst(const JC_String* const str, size_t start, size_t end);

wheels jc_String jcstr_subInLengthFrontWithCh(const char* const ch, size_t start, size_t length);

wheels jc_String jcstr_subInLengthBackWithCh(const char* const ch, size_t end, size_t length);

wheels jc_String jcstr_subInRangeWithCh(const char* const ch, size_t start, size_t end);

wheels jc_String jcstr_replaceFirstWithInst(const JC_String* const str,const char* const target, const JC_String* const replacement);

wheels jc_String jcstr_replaceAllWithInst(const JC_String* const str, const char* const target, const JC_String* const replacement);

wheels jc_String jcstr_replaceFirstWithCh(const JC_String* const str, const char* const target, const char* const replacement);

wheels jc_String jcstr_replaceAllWithCh(const JC_String* const str, const char* const target, const char* const replacement);

wheels bool jcstr_startWithCh(const JC_String* const str, const char* const prefix);

wheels bool jcstr_endWithCh(const JC_String* const str, const char* const suffix);

wheels bool jcstr_startWithInst(const JC_String* const str, const JC_String* const prefix);

wheels bool jcstr_endWithInst(const JC_String* const str, const JC_String* const suffix);

wheels bool jcstr_isEmpty(const JC_String* const str);

wheels bool jcstr_isBlank(const JC_String* const str, JC_SpaceMode mode);

wheels bool jcstr_equalsWithInst(const JC_String* const str1, const JC_String* const str2);

wheels bool jcstr_equalsIgnoreCaseWithInst(const JC_String* const str1, const JC_String* const str2, JC_SpaceMode mode);

wheels bool jcstr_equalsWithCh(const JC_String* const str, const char* const ch);

wheels bool jcstr_equalsIgnoreCaseWithCh(const JC_String* const str, const char* const ch, JC_SpaceMode mode);

wheels bool jcstr_containsWithInst(const JC_String* const str1, const JC_String* const str2);

wheels bool jcstr_containsIgnoreCaseWithInst(const JC_String* const str1, const JC_String* const str2, JC_SpaceMode mode);

wheels bool jcstr_containsWithCh(const JC_String* const str, const char* const ch);

wheels bool jcstr_containsIgnoreCaseWithCh(const JC_String* const str, const char* const ch, JC_SpaceMode mode);

wheels int jcstr_compareWithInst(const JC_String* const str1, const JC_String* const str2, int* result);

wheels int jcstr_compareIgnoreCaseWithInst(const JC_String* const str1, const JC_String* const str2, int* result, JC_SpaceMode mode);

wheels int jcstr_compareWithCh(const JC_String* const str, const char* const ch, int* result);

wheels int jcstr_compareIgnoreCaseWithCh(const JC_String* const str, const char* const ch, int* result, JC_SpaceMode mode);


wheels char* jcstr_toCharArray(const JC_String* const str);

wheels int jcstr_toInt(const JC_String* const str, int* result, JC_SpaceMode mode);

wheels int jcstr_toLong(const JC_String* const str, long* result, JC_SpaceMode mode);

wheels int jcstr_toLongLong(const JC_String* const str, long long* result, JC_SpaceMode mode);

wheels int jcstr_toFloat(const JC_String* const str, float* result, JC_SpaceMode mode);

wheels int jcstr_toDouble(const JC_String* const str, double* result, JC_SpaceMode mode);

wheels jc_String jcstr_toLowerCase(const JC_String* const str, JC_SpaceMode mode);

wheels jc_String jcstr_toUpperCase(const JC_String* const str, JC_SpaceMode mode);

wheels jc_MutableString jcstr_toMutableString(const JC_String* const str);