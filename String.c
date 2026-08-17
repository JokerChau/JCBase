#include "String.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include "resources.h"
#include <errno.h>

typedef struct String {
	char* buffer;
	int status;
	size_t cachedLen;
}String;

typedef enum {
	INITIAL_STRING_SIZE = 200,
	GROWTH = 2,
}Sizes;

string newString() {
	string str = (string)malloc(sizeof(String));
	if (!str)return NULL;

	str->buffer=(char*)calloc(INITIAL_STRING_SIZE, sizeof(char));
	if (!str->buffer) {
		str->buffer = NULL;
		str->status = NULLBUFFER;
		str->cachedLen = 0;
	}
	else {
		str->cachedLen = (size_t)-1;
		str->status = AVAILABLE;
	}

	return str;
}

string newStringFrom(const char* const content) {
	bool isNull = content == NULL;

	string str = (string)malloc(sizeof(String));
	if (!str)return NULL;

	if (isNull) {
		str->buffer = NULL;
		str->status = NULLBUFFER;
		str->cachedLen = 0;
		return str;
	}

	size_t len = strlen(content);
	str->buffer = (char*)malloc(len + 1);
	if (!str->buffer) {
		str->buffer = NULL;
		str->status = NULLBUFFER;
		str->cachedLen = 0;
	}
	else {
		memcpy(str->buffer, content, len + 1);
		str->cachedLen = len;
		str->status = AVAILABLE;
	}

	return str;
}

void deleteString(string str) {
	if (!str)return;
	if (str->buffer) {
		free(str->buffer);
		str->buffer = NULL;
	}
	free(str);
}

int getStringStatus(const String* const str) {
	if (!str)return NULLSTRING;
	return str->status;
}

size_t getStringLength(const String* const str) {
	if (!str || !str->buffer)return 0;

	string mutableStr = (string)str;

	if (mutableStr->cachedLen == (size_t)-1) {
		mutableStr->cachedLen = strlen(str->buffer);
	}
	return mutableStr->cachedLen;
}

const char* getStringContent(const String* const str) {
	if (!str || !str->buffer)return NULL;
	return str->buffer;
}

bool setStringContent(String* const str, const char* const content) {
	if (!str)return false;

	str->cachedLen = (size_t)-1;

	if (!content) {
		if (str->buffer)free(str->buffer);
		str->buffer = (char*)malloc(1);
		if (str->buffer) {
			str->buffer[0] = '\0';
			str->status = AVAILABLE;
		}
		else {
			str->buffer = NULL;
			str->status = NULLBUFFER;
		}
		return true;
	}

	size_t len = strlen(content);
	char* newBuffer = (char*)malloc(len + 1);
	if (!newBuffer)return false;

	memcpy(newBuffer, content, len + 1);
	if (str->buffer)free(str->buffer);
	str->buffer = newBuffer;
	str->status = AVAILABLE;
	
	return true;
}

const char* stringStatusToArray(const int status) {
	switch (status) {
	case NULLSTRING: {
		return "null string";
	}
	case AVAILABLE: {
		return "available string";
	}
	case NULLBUFFER: {
		return "null buffer";
	}
	case EMPTYSTRING: {
		return "empty string";
	}
	case BLANKSTRING: {
		return "blank string";
	}

	case FAILEDTOCOMBINE: {
		return "failed to combine";
	}
	case WRONGSTRINGSTATUS: {
		return "wrong string status";
	}
	case FAILEDTOREALLOC: {
		return "failed to realloc";
	}

	case NULLINT: {
		return "null int pointer";
	}
	case OVERFLOWINT: {
		return "overflow int range";
	}
	case NOTINT: {
		return "not int";
	}
	case SUCCEEDTOINT: {
		return "succeed to change string into int";
	}

	case NULLLONG: {
		return "null long int pointer";
	}
	case OVERFLOWLONG: {
		return "overflow long int range";
	}
	case NOTLONG: {
		return "not long int";
	}
	case SUCCEEDTOLONG: {
		return "succeed to change string into long int";
	}

	case NULLLONGLONG: {
		return "null long long int pointer";
	}
	case OVERFLOWLONGLONG: {
		return "overflow long long int range";
	}
	case NOTLONGLONG: {
		return "not long long int";
	}
	case SUCCEEDTOLONGLONG: {
		return "succeed to change string into long long int";
	}

	case NULLFLOAT: {
		return "null float pointer";
	}
	case NOTFLOAT: {
		return "not float";
	}
	case SUCCEEDTOFLOAT: {
		return "succeed to change string into float";
	}

	case NULLDOUBLE: {
		return "null double pointer";
	}
	case NOTDOUBLE: {
		return "not double";
	}
	case SUCCEEDTODOUBLE: {
		return "succeed to change string into double";
	}

	case NULLCHAR: {
		return "null char pointer";
	}

	case IDXOUTBOUNDS: {
		return "index out of bounds";
	}
	case SUCCESSFULIDX: {
		return "succeed to use the index";
	}
	case SUCCEEDTOTRUNCATESTRING: {
		return "succeed to truncate the string";
	}
	default: {
		return "Unknown status";
	}
	}
}


string appendString(const String* const str1, const String* const str2) {
	// 如果都为空则表明要拼接两段空string，那么直接返回NULL
	if (!str1 && !str2)return NULL;
	// 如果一个为空一个不为空，则返回以另一个string的内容创建的新实例
	else if (!str1 && str2)return newStringFrom(getStringContent(str2));
	else if (!str2 && str1)return newStringFrom(getStringContent(str1));
	// 两个string都不为空

	// 如果两个buffer都是空，则说明返回内容为空，那么返回一个可用的新实例
	if ((!str1->buffer || str1->buffer[0] == '\0') && (!str2->buffer || str2->buffer[0] == '\0'))return newString();
	// 如果其中一个buffer为空，则返回以另一个string内容创建的新实例
	if (!str1->buffer || str1->buffer[0] == '\0')return newStringFrom(getStringContent(str2));
	if (!str2->buffer || str2->buffer[0] == '\0')return newStringFrom(getStringContent(str1));
	// 如果都不为空

	// 取得两方buffer长度
	size_t len1 = strlen(str1->buffer);
	size_t len2 = strlen(str2->buffer);
	char* combined = (char*)malloc(len1 + len2 + 1);
	// 如果内容结合指针申请内存失败，则返回一个状态为FAILEDTOCOMBINE的实例
	if (!combined) {
		string err = (string)malloc(sizeof(String));
		// 如果连err都分配不了，则返回NULL，大多分配不了combined的情况应该都分配不了err
		if (!err)return NULL;
		err->buffer = NULL;
		err->status = FAILEDTOCOMBINE;
		err->cachedLen = 0;
		return err;
	}

	// 将内容复制到combined
	memcpy(combined, str1->buffer, len1);
	memcpy(combined + len1, str2->buffer, len2 + 1);

	string result = (string)malloc(sizeof(String));
	if (!result) {
		free(combined);
		return NULL;
	}
	result->buffer = combined;
	result->status = AVAILABLE;
	result->cachedLen = (size_t)-1;
	return result;
}

string appendStringFrom(const String* const str, const char* const content) {
	if (!str)return NULL;

	if ((!str->buffer || str->buffer[0] == '\0') && (!content || content[0] == '\0'))return newString();
	if (!str->buffer || str->buffer[0] == '\0')return newStringFrom(content);
	if (!content || content[0] == '\0')return newStringFrom(getStringContent(str));

	size_t len1 = strlen(str->buffer);
	size_t len2 = strlen(content);
	char* combined = (char*)malloc(len1 + len2 + 1);
	if (!combined) {
		string err = (string)malloc(sizeof(String));
		if (!err)return NULL;
		err->buffer = NULL;
		err->status = FAILEDTOCOMBINE;
		err->cachedLen = 0;
		return err;
	}

	memcpy(combined, str->buffer, len1);
	memcpy(combined + len1, content, len2 + 1);

	string result = (string)malloc(sizeof(String));
	if (!result) {
		free(combined);
		return NULL;
	}
	result->buffer = combined;
	result->status = AVAILABLE;
	result->cachedLen = (size_t)-1;

	return result;
}

string reverseString(const String* const str) {
	if (!str)return NULL;
	if (!str->buffer)return newStringFrom(NULL);
	if (str->buffer[0] == '\0')return newStringFrom("");

	size_t len = strlen(str->buffer);

	string temp = (string)malloc(sizeof(String));
	if (!temp)return NULL;

	temp->buffer = (char*)malloc(len + 1);
	if (!temp->buffer) {
		temp->buffer = NULL;
		temp->status = NULLBUFFER;
		temp->cachedLen = 0;
		return temp;
	}

	memcpy(temp->buffer, str->buffer, len + 1);

	for (size_t i = 0;i < len / 2;i++) {
		char c = temp->buffer[i];
		temp->buffer[i] = temp->buffer[len - 1 - i];
		temp->buffer[len - 1 - i] = c;
	}

	temp->status = AVAILABLE;
	temp->cachedLen = len;
	return temp;
}

string* splitString(const String* const str, char delimiter, int* count) {
	// 计数器为空
	if (!count) {
		string* stringArray = (string*)malloc(sizeof(string));
		if (!stringArray)return NULL;
		stringArray[0] = newStringFrom("count is NULL\n");
		return stringArray;
	}

	// 三种置空判断
	if (!str)return NULL;
	if (!str->buffer) {
		string* stringArray = (string*)malloc(sizeof(string));
		if (!stringArray)return NULL;
		stringArray[0] = newStringFrom(NULL);
		*count = 1;
		return stringArray;
	}
	if (str->buffer[0] == '\0') {
		string* stringArray = (string*)malloc(sizeof(string));
		if (!stringArray)return NULL;
		stringArray[0] = newStringFrom("");
		*count = 1;
		return stringArray;
	}

	size_t len = strlen(str->buffer);

	char* buffer = (char*)malloc(len + 1);
	if (!buffer) {
		string* stringArray = (string*)malloc(sizeof(string));
		if (!stringArray)return NULL;
		stringArray[0] = newStringFrom("copied buffer is NULL\n");
		*count = -1;
		return stringArray;
	}

	memcpy(buffer, str->buffer, len + 1);

	int wholeCount = 1;
	for (size_t i = 0;i < len;i++)if (str->buffer[i] == delimiter)wholeCount++;

	string* stringArray = (string*)malloc(sizeof(string) * wholeCount);
	if (!stringArray) {
		free(buffer);
		return NULL;
	}
	
	size_t j = 0;
	// i表示元素下标
	for (size_t i = 0;i < wholeCount;i++) {
		size_t staPos = j;
		// 在buffer长度内循环
		while (j < len && buffer[j] != delimiter) {
			j++;
		}
		size_t tempLen = j - staPos;
		// 分配临时字符数组
		char* temp = (char*)malloc(sizeof(char) * tempLen + 1);
		if (!temp) {
			stringArray[i] = newStringFrom(NULL);
			if (j < len)j++;
			continue;
		}
		// 将buffer的片段内容复制给temp
		memcpy(temp, buffer + staPos, tempLen);
		temp[tempLen] = '\0';
		stringArray[i] = newStringFrom(temp);
		free(temp);

		if (j < len) {
			j++;
		}
	}

	free(buffer);

	return stringArray;
}

string trim(const String* const str) {
	if (!str)return NULL;
	if (!str->buffer)return newStringFrom(NULL);
	if (str->buffer[0] == '\0')return newStringFrom("");

	size_t len = getStringLength(str);
	size_t start = 0;
	while (start < len && isspace((unsigned char)str->buffer[start])) {
		start++;
	}

	if (start == len)return newString();

	size_t end = len - 1;
	while (end > start && isspace((unsigned char)str->buffer[end])) {
		end--;
	}

	size_t newLen = end - start + 1;

	string temp = (string)malloc(sizeof(String));
	if (!temp)return NULL;

	temp->buffer = (char*)malloc(newLen + 1);
	if (!temp->buffer) {
		temp->buffer = NULL;
		temp->status = NULLBUFFER;
		temp->cachedLen = 0;
		return temp;
	}

	memcpy(temp->buffer, str->buffer + start, newLen);
	temp->buffer[newLen] = '\0';

	temp->status = AVAILABLE;
	temp->cachedLen = newLen;
	return temp;
}

int charAt(const String* const str, size_t index, char* result) {
	if (!result)return NULLCHAR;
	if (!str) {
		return NULLSTRING;
	}
	if (!str->buffer) {
		return NULLBUFFER;
	}
	if (index >= getStringLength(str))return IDXOUTBOUNDS;

	*result = str->buffer[index];

	return SUCCESSFULIDX;
}

int changeAt(String* const str, size_t index, char ch) {
	if (!str) {
		return NULLSTRING;
	}
	if (!str->buffer) {
		return NULLBUFFER;
	}
	if (index >= getStringLength(str))return IDXOUTBOUNDS;

	str->buffer[index] = ch;
	if (ch == '\0') {
		str->cachedLen = index;
		return SUCCEEDTOTRUNCATESTRING;
	}

	return SUCCESSFULIDX;
}

bool isEmpty(const String* const str) {
	return !str || !str->buffer || str->buffer[0] == '\0' ? true : false;
}

bool isBlank(const String* const str) {
	if (!str || !str->buffer)return true;

	for (const char* p = str->buffer;*p;p++) {
		if (!isspace((unsigned char)*p)) {
			return false;
		}
	}

	return true;
}

bool equals(const String* const str1, const String* const str2) {
	if (str1 == str2)return true;
	if (!str1 || !str2)return false;

	if ((!str1->buffer || str1->buffer[0] == '\0') && (!str2->buffer || str2->buffer[0] == '\0'))return true;
	if ((!str1->buffer || str1->buffer[0] == '\0') || (!str2->buffer || str2->buffer[0] == '\0'))return false;

	return strcmp(str1->buffer, str2->buffer) == 0;
}

bool equalsFrom(const String* const str, const char* const ch) {
	if ((!str || !str->buffer) && !ch)return true;
	if (((!str || !str->buffer) && ch) || ((str && str->buffer) && !ch))return false;
	return strcmp(str->buffer, ch) == 0;
}

bool contains(const String* const str1, const String* const str2) {
	if (!str2)return true;
	if (!str2->buffer)return str1 != NULL;
	if (str2->buffer[0] == '\0') {
		if (!str1||!str1->buffer)return false;
		return true;
	}

	if (!str1 || !str1->buffer || str1->buffer[0] == '\0')return false;

	return strstr(str1->buffer, str2->buffer) != NULL;
}

bool containsFrom(const String* const str, const char* ch) {
	if (!ch)return str != NULL;
	if (ch[0] == '\0') {
		if (!str || !str->buffer)return false;
		return true;
	}

	if (!str || !str->buffer || str->buffer[0] == '\0')return false;

	return strstr(str->buffer, ch) != NULL;
}


char* toArray(const String* const str) {
	if (!str || !str->buffer)return NULL;
	if (str->buffer[0] == '\0') {
		char* temp = (char*)calloc(1, sizeof(char));
		if (!temp)return NULL;
		temp[0] = '\0';
		return temp;
	}

	size_t len = getStringLength(str) + 1;
	char* content = (char*)malloc(len);
	return content ? memcpy(content, str->buffer, len) : NULL;
}

int toInt(const String* const str, int* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return EMPTYSTRING;
	if (getStringStatus(str))return WRONGSTRINGSTATUS;
	if (!result)return NULLINT;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return BLANKSTRING;
	}

	bool isNegative = false;
	int tempNum = 0;
	int numCount = 0;
	size_t i = start;

	if (p[i] == '-' || p[i] == '+') {
		if (p[i] == '-')isNegative = true;
		i++;
	}

	for (;i < end;i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			int digit = p[i] - '0';

			if (tempNum < (INT_MIN + digit) / 10) {
				return OVERFLOWINT;
			}

			tempNum = 10 * tempNum - digit;
			numCount++;
		}
		else {
			return NOTINT;
		}
	}

	if (numCount) {
		if (isNegative)*result = tempNum;
		else *result = -tempNum;
		return SUCCEEDTOINT;
	}
	else {
		return NOTINT;
	}
}

int toLong(const String* const str, long* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return EMPTYSTRING;
	if (getStringStatus(str))return WRONGSTRINGSTATUS;
	if (!result)return NULLLONG;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return BLANKSTRING;
	}

	bool isNegative = false;
	long tempNum = 0;
	int numCount = 0;
	size_t i = start;

	if (p[i] == '-' || p[i] == '+') {
		if (p[i] == '-')isNegative = true;
		i++;
	}

	for (;i < end;i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			long digit = p[i] - '0';

			if (tempNum < (LONG_MIN + digit) / 10) {
				return OVERFLOWLONG;
			}

			tempNum = 10 * tempNum - digit;
			numCount++;
		}
		else {
			return NOTLONG;
		}
	}

	if (numCount) {
		if (isNegative)*result = tempNum;
		else *result = -tempNum;
		return SUCCEEDTOLONG;
	}
	else {
		return NOTLONG;
	}
}

int toLongLong(const String* const str, long long* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return EMPTYSTRING;
	if (getStringStatus(str))return WRONGSTRINGSTATUS;
	if (!result)return NULLLONGLONG;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return BLANKSTRING;
	}

	bool isNegative = false;
	long long tempNum = 0;
	int numCount = 0;
	size_t i = start;

	if (p[i] == '-' || p[i] == '+') {
		if (p[i] == '-')isNegative = true;
		i++;
	}

	for (;i < end;i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			long long digit = p[i] - '0';

			if (tempNum < (LLONG_MIN + digit) / 10) {
				return OVERFLOWLONGLONG;
			}

			tempNum = 10 * tempNum - digit;
			numCount++;
		}
		else {
			return NOTLONGLONG;
		}
	}

	if (numCount) {
		if (isNegative)*result = tempNum;
		else *result = -tempNum;
		return SUCCEEDTOLONGLONG;
	}
	else {
		return NOTLONGLONG;
	}
}

int toFloat(const String* const str, float* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return EMPTYSTRING;
	if (getStringStatus(str))return WRONGSTRINGSTATUS;
	if (!result)return NULLFLOAT;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return BLANKSTRING;
	}

	size_t i = start;
	bool seenDigit = false;
	bool seenDot = false;

	if (p[i] == '-' || p[i] == '+') i++;

	if (i < end && p[i] == '.') {
		return NOTFLOAT;   // 不允许 ".5" 这种没有整数部分的格式
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			seenDigit = true;
		}
		else if (p[i] == '.' && !seenDot) {
			seenDot = true;
		}
		else {
			return NOTFLOAT;
		}
	}

	if (!seenDigit) return NOTFLOAT;

	// 调用标准库转换，从 p + start 开始
	char* endptr = NULL;
	errno = 0;
	float value = strtof(p + start, &endptr);

	// 确认解析到有效片段末尾
	if (endptr != p + end) {
		return NOTFLOAT;
	}

	if (errno == ERANGE) {
		return OVERFLOWFLOAT;
	}

	*result = value;
	return SUCCEEDTOFLOAT;
}

int toDouble(const String* const str, double* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return EMPTYSTRING;
	if (getStringStatus(str))return WRONGSTRINGSTATUS;
	if (!result)return NULLDOUBLE;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return BLANKSTRING;
	}

	size_t i = start;
	bool seenDigit = false;
	bool seenDot = false;

	if (p[i] == '-' || p[i] == '+') i++;

	if (i < end && p[i] == '.') {
		return NOTDOUBLE;   // 不允许 ".5" 这种没有整数部分的格式
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			seenDigit = true;
		}
		else if (p[i] == '.' && !seenDot) {
			seenDot = true;
		}
		else {
			return NOTDOUBLE;
		}
	}

	if (!seenDigit) return NOTDOUBLE;

	// 调用标准库转换，从 p + start 开始
	char* endptr = NULL;
	errno = 0;
	double value = strtod(p + start, &endptr);

	// 确认解析到有效片段末尾
	if (endptr != p + end) {
		return NOTDOUBLE;
	}

	if (errno == ERANGE) {
		return OVERFLOWDOUBLE;
	}

	*result = value;
	return SUCCEEDTODOUBLE;
}

string toLowerCase(const String* const str) {
	if (!str)return NULL;
	if (!str->buffer)return newStringFrom(NULL);
	if (str->buffer[0] == '\0')return newStringFrom("");

	size_t len = getStringLength(str);

	string newOne = (string)malloc(sizeof(String));
	if (!newOne)return NULL;

	newOne->buffer = (char*)malloc(len + 1);
	if (!newOne->buffer) {
		newOne->buffer = NULL;
		newOne->status = NULLBUFFER;
		newOne->cachedLen = 0;
		return newOne;
	}

	memcpy(newOne->buffer, str->buffer, len + 1);

	for (size_t i = 0;i < len;i++) {
		newOne->buffer[i] = tolower((unsigned char)newOne->buffer[i]);
	}

	newOne->status = AVAILABLE;
	newOne->cachedLen = (size_t)-1;
	return newOne;
}

string toUpperCase(const String* const str) {
	if (!str)return NULL;
	if (!str->buffer)return newStringFrom(NULL);
	if (str->buffer[0] == '\0')return newStringFrom("");

	size_t len = getStringLength(str);

	string newOne = (string)malloc(sizeof(String));
	if (!newOne)return NULL;

	newOne->buffer = (char*)malloc(len + 1);
	if (!newOne->buffer) {
		newOne->buffer = NULL;
		newOne->status = NULLBUFFER;
		newOne->cachedLen = 0;
		return newOne;
	}

	memcpy(newOne->buffer, str->buffer, len + 1);

	for (size_t i = 0;i < len;i++) {
		newOne->buffer[i] = toupper((unsigned char)newOne->buffer[i]);
	}

	newOne->status = AVAILABLE;
	newOne->cachedLen = (size_t)-1;
	return newOne;
}


string readLine() {
	string str = (string)malloc(sizeof(String));
	if (!str) {
		return NULL;
	}

	str->buffer = (char*)calloc(INITIAL_STRING_SIZE, sizeof(char));
	if (!str->buffer) {
		str->status = NULLBUFFER;
		str->buffer = NULL;
		str->cachedLen = 0;
		return str;
	}

	size_t capacity = INITIAL_STRING_SIZE;
	size_t len = 0;
	int ch;

	while ((ch = getchar()) != EOF && ch != '\n') {
		if (len + 1 >= capacity) {
			size_t newCapacity = capacity * GROWTH;
			char* newBuffer = (char*)realloc(str->buffer, newCapacity);
			if (!newBuffer) {
				free(str->buffer);
				str->buffer = NULL;
				str->status = FAILEDTOREALLOC;
				str->cachedLen = 0;
				return str;
			}
			str->buffer = newBuffer;
			capacity = newCapacity;
		}
		str->buffer[len++] = (char)ch;
	}

	if (len + 1 <= capacity) {
		str->buffer[len] = '\0';
	}
	else {
		str->buffer[capacity - 1] = '\0';
	}
	str->status = AVAILABLE;
	str->cachedLen = (size_t)-1;

	return str;
}

/*
* 待开发名单：
* indexOf
* subString
* replace
* start/endWith
*/