#pragma once

#include "wheel_ex.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct String String;

typedef String* string;

wheels string newString();

wheels string newStringFrom(const char* const content);

wheels void deleteString(string str);

wheels int getStringStatus(const String* const str);

wheels size_t getStringLength(const String* const str);

wheels const char* getStringContent(const String* const str);

wheels bool setStringContent(String* const str, const char* const content);

wheels const char* stringStatusToArray(const int status);


wheels string appendString(const String* const str1, const String* const str2);

wheels string appendStringFrom(const String* const str, const char* const content);

wheels string reverseString(const String* const str);

wheels string* splitString(const String* const str, char delimiter, int* count);

wheels string trim(const String* const str);

wheels int charAt(const String* const str, size_t index, char* result);

wheels int changeAt(String* const str, size_t index, char ch);

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


string readLine();