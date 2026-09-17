#pragma once

#include "wheel_ex.h"
#include <stdbool.h>
#include <stddef.h>
#include "StringSrc.h"
#include "StringBuilder.h"

typedef struct String String;

typedef String* string;

wheels string newString();

wheels string newStringFrom(const char* const content);

wheels void deleteString(string str);

wheels int getStringStatus(const String* const str);

wheels size_t getStringLength(const String* const str);

wheels char* getStringContent(const String* const str);

wheels const char* stringStatusToCharArray(const int status);


wheels string appendString(const String* const str1, const String* const str2);

wheels string appendStringFrom(const String* const str, const char* const content);

wheels string reverseString(const String* const str);

wheels int splitString(const String* const str, char delimiter, string** outArray, size_t* count);

wheels string trim(const String* const str);

wheels string clone(const String* const str);

wheels int charAt(const String* const str, size_t index, char* result);

wheels int indexOfFront(const String* const str, char ch, size_t* result);

wheels int indexOfBack(const String* const str, char ch, size_t* result);

wheels string subStringInLengthFront(const String* const str, size_t start, size_t length);

wheels string subStringInLengthBack(const String* const str, size_t end, size_t length);

wheels string subStringInRange(const String* const str, size_t start, size_t end);

wheels string subStringInLengthFrontFrom(const char* const ch, size_t start, size_t length);

wheels string subStringInLengthBackFrom(const char* const ch, size_t end, size_t length);

wheels string subStringInRangeFrom(const char* const ch, size_t start, size_t end);

wheels string replaceFirst(const String* const str,const char* const target, const String* const replacement);

wheels string replaceAll(const String* const str, const char* const target, const String* const replacement);

wheels string replaceFirstFrom(const String* const str, const char* const target, const char* const replacement);

wheels string replaceAllFrom(const String* const str, const char* const target, const char* const replacement);

wheels bool startWith(const String* const str, const char* const prefix);

wheels bool endWith(const String* const str, const char* const suffix);

wheels bool startWithString(const String* const str, const String* const prefix);

wheels bool endWithString(const String* const str, const String* const suffix);

wheels bool isEmpty(const String* const str);

wheels bool isBlank(const String* const str);

wheels bool equals(const String* const str1, const String* const str2);

wheels bool equalsFrom(const String* const str, const char* const ch);

wheels bool contains(const String* const str1, const String* const str2);

wheels bool containsFrom(const String* const str, const char* const ch);


wheels char* toArray(const String* const str);

wheels int toInt(const String* const str, int* result);

wheels int toLong(const String* const str, long* result);

wheels int toLongLong(const String* const str, long long* result);

wheels int toFloat(const String* const str, float* result);

wheels int toDouble(const String* const str, double* result);

wheels string toLowerCase(const String* const str);

wheels string toUpperCase(const String* const str);

wheels stringBuilder stringToSb(const String* const str);