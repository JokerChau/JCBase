#include "JC_String.h"
#include "JC_Scanner.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <stdint.h>

typedef struct String {
	char* buffer;
	int status;
	size_t cachedLen;
}String;

typedef struct StringBuilder {
	char* buffer;
	int status;
	size_t len;
	size_t cap;
}StringBuilder;

typedef enum {
	INITIAL_STRING_SIZE=200,
	INITIAL_SB_SIZE=200,
	GROWTH = 2,
}Sizes;

/* =============================================================== */
// 默认构造器
string newString() {
	return newStringFrom("");
}

// 通过char*构造string
string newStringFrom(const char* const content) {
	bool isNull = content == NULL;

	string str = (string)malloc(sizeof(String));
	if (!str)return NULL;

	if (isNull) {
		str->buffer = NULL;
		str->status = STR_NULLBUFFER;
		str->cachedLen = 0;
		return str;
	}

	// 获取char*长度
	size_t len = strlen(content);
	// 为buffer申请内存
	str->buffer = (char*)malloc(len + 1);
	if (!str->buffer) {
		str->buffer = NULL;
		str->status = STR_NULLBUFFER;
		str->cachedLen = 0;
	}
	// 若为buffer申请内存成功则更新状态并将长度设置为已知的len
	else {
		memcpy(str->buffer, content, len + 1);
		str->cachedLen = len;
		str->status = STR_AVAILABLE;
	}

	return str;
}
/* =============================================================== */



/* =============================================================== */
// 析构，可传NULL
void deleteString(string str) {
	if (!str)return;
	if (str->buffer) {
		free(str->buffer);
		str->buffer = NULL;
	}
	free(str);
}
/* =============================================================== */



/* =============================================================== */
// 状态返回器，在使用实例前一定调用此函数检查实例状态是否可用，使用方法：if(!getStringStatus(str)){ // 该实例状态正常}
int getStringStatus(const String* const str) {
	if (!str)return STR_NULL;
	return str->status;
}
/* =============================================================== */



/* =============================================================== */
// 获取实例长度
size_t getStringLength(const String* const str) {
	if (!str)return 0;
	return str->cachedLen;
}
/* =============================================================== */



/* =============================================================== */
// 获取实例内容转化为char*
char* getStringContent(const String* const str) {
	if (!str || !str->buffer)return NULL;
	char* temp = (char*)malloc(str->cachedLen + 1);
	if (!temp)return NULL;
	memcpy(temp, str->buffer, str->cachedLen + 1);
	temp[str->cachedLen] = '\0';
	return temp;
}
/* =============================================================== */



/* =============================================================== */
const char* stringStatusToCharArray(const int status) {
	switch (status) {
	case STR_NULL: {
		return "null string";
	}
	case STR_AVAILABLE: {
		return "available string";
	}
	case STR_NULLBUFFER: {
		return "null buffer";
	}
	case STR_EMPTYBUFFER: {
		return "buffer content null";
	}
	case STR_EMPTY: {
		return "empty string";
	}
	case STR_BLANK: {
		return "blank string";
	}

	case STR_FAILEDTOCOMBINE: {
		return "failed to combine";
	}
	case STR_WRONGSTATUS: {
		return "wrong string status";
	}
	case STR_FAILEDTOREALLOC: {
		return "failed to realloc";
	}
	case STR_FAILEDTOALLOC: {
		return "failed to alloc";
	}

	case STR_NULLINT: {
		return "null int pointer";
	}
	case STR_OVERFLOWINT: {
		return "overflow int range";
	}
	case STR_NOTINT: {
		return "not int";
	}
	case STR_SUCCEEDTOINT: {
		return "succeed to change string into int";
	}

	case STR_NULLLONG: {
		return "null long int pointer";
	}
	case STR_OVERFLOWLONG: {
		return "overflow long int range";
	}
	case STR_NOTLONG: {
		return "not long int";
	}
	case STR_SUCCEEDTOLONG: {
		return "succeed to change string into long int";
	}

	case STR_NULLLONGLONG: {
		return "null long long int pointer";
	}
	case STR_OVERFLOWLONGLONG: {
		return "overflow long long int range";
	}
	case STR_NOTLONGLONG: {
		return "not long long int";
	}
	case STR_SUCCEEDTOLONGLONG: {
		return "succeed to change string into long long int";
	}

	case STR_NULLFLOAT: {
		return "null float pointer";
	}
	case STR_OVERFLOWFLOAT: {
		return "overflow float range";
	}
	case STR_NOTFLOAT: {
		return "not float";
	}
	case STR_SUCCEEDTOFLOAT: {
		return "succeed to change string into float";
	}

	case STR_NULLDOUBLE: {
		return "null double pointer";
	}
	case STR_OVERFLOWDOUBLE: {
		return "overflow double range";
	}
	case STR_NOTDOUBLE: {
		return "not double";
	}
	case STR_SUCCEEDTODOUBLE: {
		return "succeed to change string into double";
	}

	case STR_NULLCHAR: {
		return "null char pointer";
	}
	case STR_NULLARRAY: {
		return "null string array";
	}

	case STR_IDXOUTBOUNDS: {
		return "index out of bounds";
	}
	case STR_SUCCESSFULIDX: {
		return "succeed to use the index";
	}
	case STR_SUCCEEDTOTRUNCATE: {
		return "succeed to truncate the string";
	}
	case STR_SUCCEEDTOFINDCH: {
		return "succeed to find the char you want";
	}
	case STR_WITHOUTCH: {
		return "there is no char you want here";
	}
	case STR_INVALIDSTART: {
		return "invalid start index";
	}
	case STR_INVALIDEND: {
		return "invalid end index";
	}
	case STR_INVALIDIDX: {
		return "invalid index";
	}
	case STR_INVALIDREPLACEMENT: {
		return "what you want to replace with is invalid";
	}

	case STR_NULLCOUNT: {
		return "count ptr null";
	}
	case STR_SUCCESSFULOP: {
		return "successful operation";
	}
	case STR_PARTIALSPLIT: {
		return "string partial split";
	}
	default: {
		return "Unknown status";
	}
	}
}
/* =============================================================== */





/* =============================================================== */
string appendString(const String* const str1, const String* const str2) {
	// 如果都为空则表明要拼接两段空string，那么直接返回NULL
	if (!str1 && !str2)return NULL;
	// 如果一个为空一个不为空，则返回以另一个string的内容创建的新实例
	else if (!str1 && str2)return newStringFrom(str2->buffer);
	else if (!str2 && str1)return newStringFrom(str1->buffer);
	// 两个string都不为空

	// 如果两个buffer都是空，则说明返回内容为空，那么返回一个可用的新实例
	if ((!str1->buffer || str1->buffer[0] == '\0') && (!str2->buffer || str2->buffer[0] == '\0'))return newString();
	// 如果其中一个buffer为空，则返回以另一个string内容创建的新实例
	if (!str1->buffer || str1->buffer[0] == '\0')return newStringFrom(str2->buffer);
	if (!str2->buffer || str2->buffer[0] == '\0')return newStringFrom(str1->buffer);
	// 如果都不为空

	// 取得两方buffer长度
	size_t len1 = strlen(str1->buffer);
	size_t len2 = strlen(str2->buffer);
	char* combined = (char*)malloc(len1 + len2 + 1);
	// 如果内容结合指针申请内存失败，则返回一个状态为STR_FAILEDTOCOMBINE的实例
	if (!combined) {
		string err = (string)malloc(sizeof(String));
		// 如果连err都分配不了，则返回NULL，大多分配不了combined的情况应该都分配不了err
		if (!err)return NULL;
		err->buffer = NULL;
		err->status = STR_FAILEDTOCOMBINE;
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
	result->status = STR_AVAILABLE;
	result->cachedLen = len1 + len2;
	return result;
}

string appendStringFrom(const String* const str, const char* const content) {
	if (!str)return NULL;

	if ((!str->buffer || str->buffer[0] == '\0') && (!content || content[0] == '\0'))return newString();
	if (!str->buffer || str->buffer[0] == '\0')return newStringFrom(content);
	if (!content || content[0] == '\0')return newStringFrom(str->buffer);

	size_t len1 = strlen(str->buffer);
	size_t len2 = strlen(content);
	char* combined = (char*)malloc(len1 + len2 + 1);
	if (!combined) {
		string err = (string)malloc(sizeof(String));
		if (!err)return NULL;
		err->buffer = NULL;
		err->status = STR_FAILEDTOCOMBINE;
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
	result->status = STR_AVAILABLE;
	result->cachedLen = len1 + len2;

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
		temp->status = STR_NULLBUFFER;
		temp->cachedLen = 0;
		return temp;
	}

	memcpy(temp->buffer, str->buffer, len + 1);

	for (size_t i = 0; i < len / 2; i++) {
		char c = temp->buffer[i];
		temp->buffer[i] = temp->buffer[len - 1 - i];
		temp->buffer[len - 1 - i] = c;
	}

	temp->status = STR_AVAILABLE;
	temp->cachedLen = len;
	return temp;
}

int splitString(const String* const str, char delimiter, string** outArray, size_t* count) {
	if (!outArray)return STR_NULLARRAY;
	// 计数器为空
	if (!count)return STR_NULLCOUNT;

	*outArray = NULL;
	*count = 0;

	// 三种置空判断
	if (!str)return STR_NULL;
	if (!str->buffer)return STR_NULLBUFFER;
	if (str->buffer[0] == '\0') {
		string* stringArray = (string*)malloc(sizeof(string));
		if (!stringArray)return STR_FAILEDTOALLOC;
		stringArray[0] = newStringFrom("");
		if (!stringArray[0]) {
			free(stringArray);
			return STR_FAILEDTOALLOC;
		}
		*count = 1;
		*outArray = stringArray;
		return STR_SUCCESSFULOP;
	}

	size_t len = strlen(str->buffer);

	size_t wholeCount = 1;
	for (size_t i = 0; i < len; i++)if (str->buffer[i] == delimiter)wholeCount++;

	string* stringArray = (string*)malloc(sizeof(string) * wholeCount);
	if (!stringArray) {
		return STR_FAILEDTOALLOC;
	}

	bool hasNull = false;
	size_t j = 0;
	// i表示元素下标
	for (size_t i = 0; i < wholeCount; i++) {
		size_t staPos = j;
		// 在buffer长度内循环
		while (j < len && str->buffer[j] != delimiter) {
			j++;
		}
		size_t tempLen = j - staPos;
		// 分配临时字符数组
		char* temp = (char*)malloc(sizeof(char) * tempLen + 1);
		if (!temp) {
			stringArray[i] = newStringFrom(NULL);
			hasNull = true;
			if (j < len)j++;
			continue;
		}
		// 将buffer的片段内容复制给temp
		memcpy(temp, str->buffer + staPos, tempLen);
		temp[tempLen] = '\0';
		stringArray[i] = newStringFrom(temp);
		free(temp);

		if (!stringArray[i])hasNull = true;

		if (j < len) {
			j++;
		}
	}

	*outArray = stringArray;
	*count = wholeCount;

	return hasNull ? STR_PARTIALSPLIT : STR_SUCCESSFULOP;
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
		temp->status = STR_NULLBUFFER;
		temp->cachedLen = 0;
		return temp;
	}

	memcpy(temp->buffer, str->buffer + start, newLen);
	temp->buffer[newLen] = '\0';

	temp->status = STR_AVAILABLE;
	temp->cachedLen = newLen;
	return temp;
}

string clone(const String* const str) {
	if (!str)return NULL;
	return newStringFrom(str->buffer);
}

int charAt(const String* const str, size_t index, char* result) {
	if (!result)return STR_NULLCHAR;
	if (!str) {
		return STR_NULL;
	}
	if (!str->buffer) {
		return STR_NULLBUFFER;
	}
	if (index >= getStringLength(str))return STR_IDXOUTBOUNDS;

	*result = str->buffer[index];

	return STR_SUCCESSFULIDX;
}

int indexOfFront(const String* const str, char ch, size_t* result) {
	if (!result)return STR_NULLINT;
	if (!str)return STR_NULL;
	if (!str->buffer)return STR_NULLBUFFER;

	const char* found = strchr(str->buffer, ch);
	if (found) {
		*result = (size_t)(found - str->buffer);
		return STR_SUCCEEDTOFINDCH;
	}
	return STR_WITHOUTCH;
}

int indexOfBack(const String* const str, char ch, size_t* result) {
	if (!result)return STR_NULLINT;
	if (!str)return STR_NULL;
	if (!str->buffer)return STR_NULLBUFFER;
	for (ptrdiff_t i = (ptrdiff_t)getStringLength(str) - 1; i >= 0; --i) {
		if (str->buffer[i] == ch) {
			*result = (size_t)i;
			return STR_SUCCEEDTOFINDCH;
		}
	}
	return STR_WITHOUTCH;
}

string subStringInLengthFront(const String* const str, size_t start, size_t length) {
	if (!str)return NULL;
	if (!str->buffer)return newStringFrom(NULL);
	if (str->buffer[0] == '\0' || !length)return newStringFrom("");

	size_t len = getStringLength(str);
	if (start >= len) {
		string temp = newStringFrom("");
		if (temp)temp->status = STR_INVALIDSTART;
		return temp;
	}

	if (length - 1 >= len - start)length = len - start;

	string temp = (string)malloc(sizeof(String));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = STR_NULLBUFFER;
		temp->cachedLen = 0;
		return temp;
	}
	memcpy(temp->buffer, str->buffer + start, length);
	temp->buffer[length] = '\0';
	temp->status = STR_AVAILABLE;
	temp->cachedLen = length;
	return temp;
}

string subStringInLengthBack(const String* const str, size_t end, size_t length) {
	if (!str)return NULL;
	if (!str->buffer)return newStringFrom(NULL);
	if (str->buffer[0] == '\0' || !length)return newStringFrom("");

	size_t len = getStringLength(str);
	if (end >= len) {
		string temp = newStringFrom("");
		if (temp)temp->status = STR_INVALIDEND;
		return temp;
	}

	if ((ptrdiff_t)end + 1 - (ptrdiff_t)length < 0)length = end + 1;

	string temp = (string)malloc(sizeof(String));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = STR_NULLBUFFER;
		temp->cachedLen = 0;
		return temp;
	}
	memcpy(temp->buffer, str->buffer + (end - length + 1), length);
	temp->buffer[length] = '\0';
	temp->status = STR_AVAILABLE;
	temp->cachedLen = length;
	return temp;
}

string subStringInRange(const String* const str, size_t start, size_t end) {
	if (!str)return NULL;
	if (!str->buffer)return newStringFrom(NULL);
	if (str->buffer[0] == '\0' || start == end)return newStringFrom("");

	if (start > end) {
		string temp = newStringFrom("");
		if (temp)temp->status = STR_INVALIDIDX;
		return temp;
	}

	size_t len = getStringLength(str);
	if (start >= len || end > len) {
		string temp = newStringFrom("");
		if (temp)temp->status = STR_IDXOUTBOUNDS;
		return temp;
	}

	size_t length = end - start;
	string temp = (string)malloc(sizeof(String));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = STR_NULLBUFFER;
		temp->cachedLen = 0;
		return temp;
	}
	memcpy(temp->buffer, str->buffer + start, length);
	temp->buffer[length] = '\0';
	temp->status = STR_AVAILABLE;
	temp->cachedLen = length;
	return temp;
}

string subStringInLengthFrontFrom(const char* const ch, size_t start, size_t length) {
	if (!ch)return newStringFrom(NULL);
	if (ch[0] == '\0' || !length)return newStringFrom("");

	size_t len = strlen(ch);
	if (start >= len) {
		string temp = newStringFrom("");
		if (temp)temp->status = STR_INVALIDSTART;
		return temp;
	}

	if (length - 1 >= len - start)length = len - start;

	string temp = (string)malloc(sizeof(String));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = STR_NULLBUFFER;
		temp->cachedLen = 0;
		return temp;
	}
	memcpy(temp->buffer, ch + start, length);
	temp->buffer[length] = '\0';
	temp->status = STR_AVAILABLE;
	temp->cachedLen = length;
	return temp;
}

string subStringInLengthBackFrom(const char* const ch, size_t end, size_t length) {
	if (!ch)return newStringFrom(NULL);
	if (ch[0] == '\0' || !length)return newStringFrom("");

	size_t len = strlen(ch);
	if (end >= len) {
		string temp = newStringFrom("");
		if (temp)temp->status = STR_INVALIDEND;
		return temp;
	}

	if ((ptrdiff_t)end + 1 - (ptrdiff_t)length < 0)length = end + 1;

	string temp = (string)malloc(sizeof(String));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = STR_NULLBUFFER;
		temp->cachedLen = 0;
		return temp;
	}
	memcpy(temp->buffer, ch + (end - length + 1), length);
	temp->buffer[length] = '\0';
	temp->status = STR_AVAILABLE;
	temp->cachedLen = length;
	return temp;
}

string subStringInRangeFrom(const char* const ch, size_t start, size_t end) {
	if (!ch)return newStringFrom(NULL);
	if (ch[0] == '\0' || start == end)return newStringFrom("");

	if (start > end) {
		string temp = newStringFrom("");
		if (temp)temp->status = STR_INVALIDIDX;
		return temp;
	}

	size_t len = strlen(ch);
	if (start >= len || end > len) {
		string temp = newStringFrom("");
		if (temp)temp->status = STR_IDXOUTBOUNDS;
		return temp;
	}

	size_t length = end - start;
	string temp = (string)malloc(sizeof(String));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = STR_NULLBUFFER;
		temp->cachedLen = 0;
		return temp;
	}
	memcpy(temp->buffer, ch + start, length);
	temp->buffer[length] = '\0';
	temp->status = STR_AVAILABLE;
	temp->cachedLen = length;
	return temp;
}

string replaceFirst(const String* const str, const char* const target, const String* const replacement) {
	if (!str)return NULL;
	if (!replacement) {
		string err = newStringFrom(str->buffer);
		if (err)err->status = STR_INVALIDREPLACEMENT;
		return err;
	}
	return replaceFirstFrom(str, target, replacement->buffer);
}

string replaceAll(const String* const str, const char* const target, const String* const replacement) {
	if (!str)return NULL;
	if (!replacement) {
		string err = newStringFrom(str->buffer);
		if (err)err->status = STR_INVALIDREPLACEMENT;
		return err;
	}
	return replaceAllFrom(str, target, replacement->buffer);
}

string replaceFirstFrom(const String* const str, const char* const target, const char* const replacement) {
	if (!str)return NULL;

	// 如果都为NULL
	if (!str->buffer && !target)return newStringFrom(replacement);
	// 如果只有buffer或target为NULL
	if (!str->buffer || !target)return newStringFrom(str->buffer);

	if (str->buffer[0] == '\0' && target[0] == '\0')return newStringFrom(replacement);
	if (str->buffer[0] == '\0' || target[0] == '\0')return newStringFrom(str->buffer);

	if (!replacement) {
		string err = newStringFrom(str->buffer);
		if (err)err->status = STR_INVALIDREPLACEMENT;
		return err;
	}

	char* indexChar = strstr(str->buffer, target);
	if (!indexChar)return newStringFrom(str->buffer);
	size_t index = (size_t)(indexChar - str->buffer);

	size_t targetLen = strlen(target);
	size_t replaceLen = strlen(replacement);
	size_t strLen = getStringLength(str);

	if (targetLen == replaceLen) {
		string tempStr = newStringFrom(str->buffer);
		if (getStringStatus(tempStr))return tempStr;
		memcpy(tempStr->buffer + index, replacement, replaceLen);
		return tempStr;
	}

	size_t preLen = index;
	size_t sufLen = strLen - preLen - targetLen;
	size_t newLen = preLen + replaceLen + sufLen;

	char* newBuffer = (char*)malloc(newLen + 1);
	if (!newBuffer) {
		return newStringFrom(NULL);
	}

	char* ptr = newBuffer;

	memcpy(ptr, str->buffer, preLen);
	ptr += preLen;

	memcpy(ptr, replacement, replaceLen);
	ptr += replaceLen;

	memcpy(ptr, indexChar + targetLen, sufLen + 1);

	string tempStr = (string)malloc(sizeof(String));
	if (!tempStr) {
		free(newBuffer);
		return NULL;
	}
	tempStr->buffer = newBuffer;
	tempStr->status = STR_AVAILABLE;
	tempStr->cachedLen = newLen;
	return tempStr;
}

string replaceAllFrom(const String* const str, const char* const target, const char* const replacement) {
	if (!str)return NULL;
	if (!str->buffer && !target)return newStringFrom(replacement);
	if (!str->buffer || !target)return newStringFrom(str->buffer);
	if (str->buffer[0] == '\0' && target[0] == '\0')return newStringFrom(replacement);
	if (str->buffer[0] == '\0' || target[0] == '\0')return newStringFrom(str->buffer);
	if (!replacement) {
		string err = newStringFrom(str->buffer);
		if (err)err->status = STR_INVALIDREPLACEMENT;
		return err;
	}

	char* src = str->buffer;
	size_t strLen = getStringLength(str);
	size_t targetLen = strlen(target);
	size_t replaceLen = strlen(replacement);

	size_t* position = NULL;
	size_t capacity = 0;
	size_t count = 0;
	char* scan = src;

	while ((scan = strstr(scan, target)) != NULL) {
		if (count >= capacity) {
			capacity += 16;
			size_t* newPos = (size_t*)realloc(position, capacity * sizeof(size_t));
			if (!newPos) {
				free(position);
				return newStringFrom(NULL);
			}
			position = newPos;
		}
		position[count++] = (size_t)(scan - src);
		scan += targetLen;
	}

	if (!count) {
		free(position);
		return newStringFrom(src);
	}

	size_t newLen;
	if (replaceLen >= targetLen) {
		newLen = strLen + count * (replaceLen - targetLen);
	}
	else {
		newLen = strLen - count * (targetLen - replaceLen);
	}
	char* newBuffer = (char*)malloc(newLen + 1);
	if (!newBuffer) {
		free(position);
		return newStringFrom(NULL);
	}

	char* ptr = newBuffer;
	size_t lastPos = 0;

	for (size_t i = 0; i < count; i++) {
		size_t pos = position[i];

		size_t copyLen = pos - lastPos;
		memcpy(ptr, src + lastPos, copyLen);
		ptr += copyLen;

		memcpy(ptr, replacement, replaceLen);
		ptr += replaceLen;

		lastPos = pos + targetLen;
	}

	memcpy(ptr, src + lastPos, strLen - lastPos + 1);

	free(position);

	string result = (string)malloc(sizeof(String));
	if (!result) {
		free(newBuffer);
		return NULL;
	}
	result->buffer = newBuffer;
	result->status = STR_AVAILABLE;
	result->cachedLen = newLen;
	return result;
}

bool startWith(const String* const str, const char* const prefix) {
	if (!str || !str->buffer)return false;

	if (!prefix || prefix[0] == '\0')return true;

	size_t strLen = getStringLength(str);
	size_t preLen = strlen(prefix);
	if (strLen < preLen)return false;

	return strncmp(str->buffer, prefix, preLen) == 0;
}

bool endWith(const String* const str, const char* const suffix) {
	if (!str || !str->buffer)return false;

	if (!suffix || suffix[0] == '\0')return true;

	size_t strLen = getStringLength(str);
	size_t sufLen = strlen(suffix);
	if (strLen < sufLen)return false;

	return strncmp(str->buffer + (strLen - sufLen), suffix, sufLen) == 0;
}

bool startWithString(const String* const str, const String* const prefix) {
	if (!prefix)return true;
	return startWith(str, prefix->buffer);
}

bool endWithString(const String* const str, const String* const suffix) {
	if (!suffix)return true;
	return endWith(str, suffix->buffer);
}

bool isEmpty(const String* const str) {
	return !str || !str->buffer || str->buffer[0] == '\0' ? true : false;
}

bool isBlank(const String* const str) {
	if (!str || !str->buffer)return true;

	for (const char* p = str->buffer; *p; p++) {
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
		if (!str1 || !str1->buffer)return false;
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
	if (!str || !str->buffer || str->buffer[0] == '\0')return STR_EMPTY;
	if (getStringStatus(str))return STR_WRONGSTATUS;
	if (!result)return STR_NULLINT;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return STR_BLANK;
	}

	bool isNegative = false;
	int tempNum = 0;
	int numCount = 0;
	size_t i = start;

	if (p[i] == '-' || p[i] == '+') {
		if (p[i] == '-')isNegative = true;
		i++;
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			int digit = p[i] - '0';

			if (tempNum < (INT_MIN + digit) / 10) {
				return STR_OVERFLOWINT;
			}

			tempNum = 10 * tempNum - digit;
			numCount++;
		}
		else {
			return STR_NOTINT;
		}
	}

	if (numCount) {
		if (isNegative)*result = tempNum;
		else *result = -tempNum;
		return STR_SUCCEEDTOINT;
	}
	else {
		return STR_NOTINT;
	}
}

int toLong(const String* const str, long* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return STR_EMPTY;
	if (getStringStatus(str))return STR_WRONGSTATUS;
	if (!result)return STR_NULLLONG;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return STR_BLANK;
	}

	bool isNegative = false;
	long tempNum = 0;
	int numCount = 0;
	size_t i = start;

	if (p[i] == '-' || p[i] == '+') {
		if (p[i] == '-')isNegative = true;
		i++;
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			long digit = p[i] - '0';

			if (tempNum < (LONG_MIN + digit) / 10) {
				return STR_OVERFLOWLONG;
			}

			tempNum = 10 * tempNum - digit;
			numCount++;
		}
		else {
			return STR_NOTLONG;
		}
	}

	if (numCount) {
		if (isNegative)*result = tempNum;
		else *result = -tempNum;
		return STR_SUCCEEDTOLONG;
	}
	else {
		return STR_NOTLONG;
	}
}

int toLongLong(const String* const str, long long* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return STR_EMPTY;
	if (getStringStatus(str))return STR_WRONGSTATUS;
	if (!result)return STR_NULLLONGLONG;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return STR_BLANK;
	}

	bool isNegative = false;
	long long tempNum = 0;
	int numCount = 0;
	size_t i = start;

	if (p[i] == '-' || p[i] == '+') {
		if (p[i] == '-')isNegative = true;
		i++;
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			long long digit = p[i] - '0';

			if (tempNum < (LLONG_MIN + digit) / 10) {
				return STR_OVERFLOWLONGLONG;
			}

			tempNum = 10 * tempNum - digit;
			numCount++;
		}
		else {
			return STR_NOTLONGLONG;
		}
	}

	if (numCount) {
		if (isNegative)*result = tempNum;
		else *result = -tempNum;
		return STR_SUCCEEDTOLONGLONG;
	}
	else {
		return STR_NOTLONGLONG;
	}
}

int toFloat(const String* const str, float* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return STR_EMPTY;
	if (getStringStatus(str))return STR_WRONGSTATUS;
	if (!result)return STR_NULLFLOAT;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return STR_BLANK;
	}

	size_t i = start;
	bool seenDigit = false;
	bool seenDot = false;

	if (p[i] == '-' || p[i] == '+') i++;

	if (i < end && p[i] == '.') {
		return STR_NOTFLOAT;   // 不允许 ".5" 这种没有整数部分的格式
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			seenDigit = true;
		}
		else if (p[i] == '.' && !seenDot) {
			seenDot = true;
		}
		else {
			return STR_NOTFLOAT;
		}
	}

	if (!seenDigit) return STR_NOTFLOAT;

	// 调用标准库转换，从 p + start 开始
	char* endptr = NULL;
	errno = 0;
	float value = strtof(p + start, &endptr);

	// 确认解析到有效片段末尾
	if (endptr != p + end) {
		return STR_NOTFLOAT;
	}

	if (errno == ERANGE) {
		return STR_OVERFLOWFLOAT;
	}

	*result = value;
	return STR_SUCCEEDTOFLOAT;
}

int toDouble(const String* const str, double* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return STR_EMPTY;
	if (getStringStatus(str))return STR_WRONGSTATUS;
	if (!result)return STR_NULLDOUBLE;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return STR_BLANK;
	}

	size_t i = start;
	bool seenDigit = false;
	bool seenDot = false;

	if (p[i] == '-' || p[i] == '+') i++;

	if (i < end && p[i] == '.') {
		return STR_NOTDOUBLE;   // 不允许 ".5" 这种没有整数部分的格式
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			seenDigit = true;
		}
		else if (p[i] == '.' && !seenDot) {
			seenDot = true;
		}
		else {
			return STR_NOTDOUBLE;
		}
	}

	if (!seenDigit) return STR_NOTDOUBLE;

	// 调用标准库转换，从 p + start 开始
	char* endptr = NULL;
	errno = 0;
	double value = strtod(p + start, &endptr);

	// 确认解析到有效片段末尾
	if (endptr != p + end) {
		return STR_NOTDOUBLE;
	}

	if (errno == ERANGE) {
		return STR_OVERFLOWDOUBLE;
	}

	*result = value;
	return STR_SUCCEEDTODOUBLE;
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
		newOne->status = STR_NULLBUFFER;
		newOne->cachedLen = 0;
		return newOne;
	}

	memcpy(newOne->buffer, str->buffer, len + 1);

	for (size_t i = 0; i < len; i++) {
		newOne->buffer[i] = tolower((unsigned char)newOne->buffer[i]);
	}

	newOne->status = STR_AVAILABLE;
	newOne->cachedLen = len;
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
		newOne->status = STR_NULLBUFFER;
		newOne->cachedLen = 0;
		return newOne;
	}

	memcpy(newOne->buffer, str->buffer, len + 1);

	for (size_t i = 0; i < len; i++) {
		newOne->buffer[i] = toupper((unsigned char)newOne->buffer[i]);
	}

	newOne->status = STR_AVAILABLE;
	newOne->cachedLen = len;
	return newOne;
}

stringBuilder stringToSb(const String* const str) {
	return newSbFrom(str->buffer);
}


string readLine() {
	string str = (string)malloc(sizeof(String));
	if (!str) {
		return NULL;
	}

	str->buffer = (char*)calloc(INITIAL_STRING_SIZE, sizeof(char));
	if (!str->buffer) {
		str->status = STR_NULLBUFFER;
		str->buffer = NULL;
		str->cachedLen = 0;
		return str;
	}

	size_t capacity = INITIAL_STRING_SIZE;
	size_t len = 0;
	int ch;

	while ((ch = getchar()) != EOF && ch != '\n') {
		if (len + 1 >= capacity) {
			if (capacity > SIZE_MAX / GROWTH) {
				free(str->buffer);
				str->buffer = NULL;
				str->status = STR_FAILEDTOREALLOC;
				str->cachedLen = 0;
				return str;
			}
			size_t newCapacity = capacity * GROWTH;
			char* newBuffer = (char*)realloc(str->buffer, newCapacity);
			if (!newBuffer) {
				free(str->buffer);
				str->buffer = NULL;
				str->status = STR_FAILEDTOREALLOC;
				str->cachedLen = 0;
				return str;
			}
			str->buffer = newBuffer;
			capacity = newCapacity;
		}
		str->buffer[len++] = (char)ch;
	}

	str->buffer[len] = '\0';
	
	str->status = STR_AVAILABLE;
	str->cachedLen = len;

	return str;
}

/*
* 待开发名单：
* 暂无
*/