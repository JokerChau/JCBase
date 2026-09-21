#pragma once

#include "wheel_ex.h"
#include "StringBuilderSrc.h"
#include <stddef.h>
#include <stdbool.h>
#include "JC_String.h"

typedef struct StringBuilder StringBuilder;
typedef StringBuilder* stringBuilder;

wheels stringBuilder newSb();

wheels stringBuilder newSbFrom(const char* const ch);

wheels void deleteSb(stringBuilder sb);

wheels int getSbStatus(const StringBuilder* const sb);

wheels size_t getSbLength(const StringBuilder* const sb);

wheels const char* getSbBuffer(const StringBuilder* const sb);

wheels const char* getSbContent(const StringBuilder* const sb);

wheels int setSbContent(StringBuilder* const str, const char* const content);

wheels int changeAt(stringBuilder str, size_t index, char ch);

wheels const char* sbStatusToCharArray(const int status);

wheels int appendSb(StringBuilder* sb, const StringBuilder* const src);

wheels int appendSbFrom(stringBuilder sb, const char* const ch);

wheels int reverseSb(stringBuilder sb);

wheels int splitSb(const StringBuilder* const str, char delimiter, stringBuilder** outArray, size_t* count);

wheels int trimSb(stringBuilder const str);

wheels stringBuilder cloneSb(const StringBuilder* const str);

wheels int charAtSb(const StringBuilder* const str, size_t index, char* result);

wheels int sbIndexOfFront(const StringBuilder* const str, char ch, size_t* result);

wheels int sbIndexOfBack(const StringBuilder* const str, char ch, size_t* result);

wheels stringBuilder subSbInLengthFront(const StringBuilder* const str, size_t start, size_t length);

wheels stringBuilder subSbInLengthBack(const StringBuilder* const str, size_t end, size_t length);

wheels stringBuilder subSbInRange(const StringBuilder* const str, size_t start, size_t end);

wheels stringBuilder subSbInLengthFrontFrom(const char* const ch, size_t start, size_t length);

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

wheels string sbToString(const String* const str);