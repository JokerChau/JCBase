#include "jsonmain.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#define jsonFseek64(fp, off, origin) _fseeki64((fp), (off), (origin))
#define jsonFtell64(fp)              _ftelli64((fp))
#else
#define jsonFseek64(fp, off, origin) fseeko((fp), (off), (origin))
#define jsonFtell64(fp)              ftello((fp))
#endif

/*
peek：看当前字符，不移动。

advance：取当前字符，position 加一。

skipWhitespace：跳过空格、换行、制表、回车。

parseValue：看当前字符，决定调哪个函数。

parseObject / parseArray / parseString / parseNumber / parseLiteral：处理具体类型。
*/

typedef enum {
	JSON_NULL,
	JSON_BOOL,
	JSON_NUMBER,
	JSON_STRING,
	JSON_ARRAY,
	JSON_OBJECT
}jsonType;

typedef struct jsonString {
	char* data;
	size_t length;
} jsonString;

typedef struct jsonArray {
	jsonNode** items;
	size_t length;
	size_t capacity;
} jsonArray;

typedef struct jsonObject {
	jsonMember** members;
	size_t length;
	size_t capacity;
} jsonObject;

typedef struct jsonNode {
	jsonType type;
	union {
		bool boolean;
		double number;
		jsonString string;
		jsonArray array;
		jsonObject object;
	}content;
}jsonNode;

typedef struct jsonMember {
	jsonString key;
	jsonNode* value;
}jsonMember;

typedef struct jsonParser {
	jsonString text;
	size_t position;
	jsonError error;
	int errnoStatus;
}jsonParser;


static void skipWhitespace(jsonParser* parser);
static jsonNode* parseValue(jsonParser* parser);
static jsonNode* parseObject(jsonParser* parser);
static jsonNode* parseArray(jsonParser* parser);
static jsonNode* parseString(jsonParser* parser);
static jsonNode* parseNumber(jsonParser* parser);
static jsonNode* parseLiteral(jsonParser* parser);
static bool readHex4(jsonParser* parser, uint32_t* out);

static jsonNode* newNullNode(void);
static jsonNode* newBoolNode(bool b);
static jsonNode* newNumberNode(double n);
static jsonNode* newStringNode(const char* s, size_t len);
static jsonNode* newArrayNode(void);
static jsonNode* newObjectNode(void);
static void jsonNodeDestroy(jsonNode* node);
static bool addItem(jsonNode* array, jsonNode* value);
static bool addMember(jsonNode* object, jsonString key, jsonNode* value);

static jsonParser* generateParser(
	char* data, size_t length,
	size_t position, jsonError error,
	int errnoStatus
) {
	jsonParser* parser = (jsonParser*)malloc(sizeof(jsonParser));
	if (!parser) {
		return NULL;
	}
	*parser = (jsonParser){
		.text = {
			.data = data,
			.length = length
		},
		.position = position,
		.error = error,
		.errnoStatus = errnoStatus
	};
	return parser;
}

static bool isJsonWhitespace(unsigned char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static int trim(jsonString* str) {
	if (!str)return JSON_NULLSTR;
	if (!str->data)return JSON_NULLSTRBUF;
	
	size_t start = 0;
	size_t end = str->length;

	while (start < end && isJsonWhitespace((unsigned char)str->data[start])) {
		start++;
	}
	while (end > start && isJsonWhitespace((unsigned char)str->data[end - 1])) {
		end--;
	}

	str->length = end - start;

	if (start > 0) {
		memmove(str->data, str->data + start, str->length);
	}

	// Null-terminate the trimmed string
	str->data[str->length] = '\0';

	return JSON_SUCCESSFULOP;
}

static void skipWhitespace(jsonParser* parser) {
	if (!parser)return;
	while (parser->position < parser->text.length &&
		isJsonWhitespace((unsigned char)parser->text.data[parser->position])) {
		parser->position++;
	}
}

static jsonNode* parseValue(jsonParser* parser) {
	skipWhitespace(parser);
	if (parser->error != JSON_SUCCESSFULOP) return NULL;

	if (parser->position >= parser->text.length) {
		parser->error = JSON_UNEXPECTEDEND;
		return NULL;
	}

	char c = parser->text.data[parser->position];

	switch (c) {
	case '{': return parseObject(parser);
	case '[': return parseArray(parser);
	case '"': return parseString(parser);
	case 't':
	case 'f':
	case 'n': return parseLiteral(parser);
	case '-':
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return parseNumber(parser);
	default:
		parser->error = JSON_INVALIDTOKEN;
		return NULL;
	}
}

static jsonNode* parseObject(jsonParser* parser) {
	// 吃掉 '{'
	parser->position++;

	// 创建对象节点
	jsonNode* node = newObjectNode();
	if (!node) {
		parser->error = JSON_ALLOCFAILED;
		return NULL;
	}

	skipWhitespace(parser);

	// 空对象 {}
	if (parser->position < parser->text.length &&
		parser->text.data[parser->position] == '}') {
		parser->position++;
		return node;
	}

	while (1) {
		skipWhitespace(parser);

		// 读 key，必须是字符串
		if (parser->position >= parser->text.length ||
			parser->text.data[parser->position] != '"') {
			parser->error = JSON_PARSEERROR;
			jsonNodeDestroy(node);
			return NULL;
		}

		jsonNode* keyNode = parseString(parser);
		if (!keyNode) {
			jsonNodeDestroy(node);
			return NULL;
		}

		// key 节点里的 string 就是 key
		jsonString key = keyNode->content.string;
		free(keyNode);  // 只需要 key 的 string，节点本身不要了

		skipWhitespace(parser);

		// 读冒号
		if (parser->position >= parser->text.length ||
			parser->text.data[parser->position] != ':') {
			parser->error = JSON_PARSEERROR;
			free(key.data);
			jsonNodeDestroy(node);
			return NULL;
		}
		parser->position++;

		// 读值（递归）
		jsonNode* value = parseValue(parser);
		if (!value) {
			free(key.data);
			jsonNodeDestroy(node);
			return NULL;
		}

		// 把 (key, value) 加进对象
		if (!addMember(node, key, value)) {
			parser->error = JSON_ALLOCFAILED;
			free(key.data);
			jsonNodeDestroy(value);
			jsonNodeDestroy(node);
			return NULL;
		}

		skipWhitespace(parser);

		if (parser->position >= parser->text.length) {
			parser->error = JSON_UNEXPECTEDEND;
			jsonNodeDestroy(node);
			return NULL;
		}

		char c = parser->text.data[parser->position];
		if (c == ',') {
			parser->position++;
			continue;
		}
		else if (c == '}') {
			parser->position++;
			return node;
		}
		else {
			parser->error = JSON_PARSEERROR;
			jsonNodeDestroy(node);
			return NULL;
		}
	}
}

static jsonNode* parseArray(jsonParser* parser) {
	parser->position++;   // 吃掉 '['

	jsonNode* node = newArrayNode();
	if (!node) {
		parser->error = JSON_ALLOCFAILED;
		return NULL;
	}

	skipWhitespace(parser);

	// 空数组 []
	if (parser->position < parser->text.length &&
		parser->text.data[parser->position] == ']') {
		parser->position++;
		return node;
	}

	while (1) {
		skipWhitespace(parser);

		jsonNode* value = parseValue(parser);
		if (!value) {
			jsonNodeDestroy(node);
			return NULL;
		}

		if (!addItem(node, value)) {
			parser->error = JSON_ALLOCFAILED;
			jsonNodeDestroy(value);
			jsonNodeDestroy(node);
			return NULL;
		}

		skipWhitespace(parser);

		if (parser->position >= parser->text.length) {
			parser->error = JSON_UNEXPECTEDEND;
			jsonNodeDestroy(node);
			return NULL;
		}

		char c = parser->text.data[parser->position];
		if (c == ',') {
			parser->position++;
			continue;
		}
		else if (c == ']') {
			parser->position++;
			return node;
		}
		else {
			parser->error = JSON_PARSEERROR;
			jsonNodeDestroy(node);
			return NULL;
		}
	}
}

static jsonNode* parseString(jsonParser* parser) {
	parser->position++;   // 吃掉开头的 '"'

	// 先扫描一遍，确定字符串长度（或直接分配一个足够大的缓冲）
	// 这里简化：假设不会太长，边读边写到一个临时缓冲
	size_t cap = 32;
	size_t len = 0;
	char* buf = (char*)malloc(cap);
	if (!buf) {
		parser->error = JSON_ALLOCFAILED;
		return NULL;
	}

	while (parser->position < parser->text.length) {
		unsigned char c = (unsigned char)parser->text.data[parser->position++];

		if (c == '"') {
			// 字符串结束
			buf[len] = '\0';
			jsonNode* node = newStringNode(buf, len);
			free(buf);
			if (!node) parser->error = JSON_ALLOCFAILED;
			return node;
		}

		if (c < 0x20) {
			free(buf);
			parser->error = JSON_PARSEERROR;
			return NULL;
		}

		if (c == '\\') {
			if (parser->position >= parser->text.length) {
				free(buf);
				parser->error = JSON_UNEXPECTEDEND;
				return NULL;
			}
			char esc = parser->text.data[parser->position++];
			switch (esc) {
			case '"':  c = '"';  break;
			case '\\': c = '\\'; break;
			case '/':  c = '/';  break;
			case 'n':  c = '\n'; break;
			case 't':  c = '\t'; break;
			case 'r':  c = '\r'; break;
			case 'b':  c = '\b'; break;
			case 'f':  c = '\f'; break;
			case 'u': {
				uint32_t cp = 0;
				if (!readHex4(parser, &cp)) {
					free(buf);
					parser->error = JSON_PARSEERROR;
					return NULL;
				}

				if (cp >= 0xD800 && cp <= 0xDBFF) {
					// 高代理项，必须跟一个低代理项
					if (parser->position + 2 >= parser->text.length ||
						parser->text.data[parser->position] != '\\' ||
						parser->text.data[parser->position + 1] != 'u') {
						free(buf);
						parser->error = JSON_PARSEERROR;
						return NULL;
					}
					parser->position += 2; // 吃掉 \u
					uint32_t low = 0;
					if (!readHex4(parser, &low) || low < 0xDC00 || low > 0xDFFF) {
						free(buf);
						parser->error = JSON_PARSEERROR;
						return NULL;
					}
					cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
				}
				else if (cp >= 0xDC00 && cp <= 0xDFFF) {
					// 低代理项，必须跟一个高代理项
					free(buf);
					parser->error = JSON_PARSEERROR;
					return NULL;
				}

				// 转成 UTF-8
				unsigned char utf8[4];
				int n;
				if (cp <= 0x7F) {
					utf8[0] = (unsigned char)cp;
					n = 1;
				}
				else if (cp <= 0x7FF) {
					utf8[0] = (unsigned char)(0xC0 | ((cp >> 6) & 0x1F));
					utf8[1] = (unsigned char)(0x80 | (cp & 0x3F));
					n = 2;
				}
				else if (cp <= 0xFFFF) {
					utf8[0] = (unsigned char)(0xE0 | ((cp >> 12) & 0x0F));
					utf8[1] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
					utf8[2] = (unsigned char)(0x80 | (cp & 0x3F));
					n = 3;
				}
				else {
					utf8[0] = (unsigned char)(0xF0 | ((cp >> 18) & 0x07));
					utf8[1] = (unsigned char)(0x80 | ((cp >> 12) & 0x3F));
					utf8[2] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
					utf8[3] = (unsigned char)(0x80 | (cp & 0x3F));
					n = 4;
				}

				// 确保 buf 够大
				if (len + (size_t)n + 1 > cap) {
					size_t newCap = cap * 2;
					while (newCap < len + (size_t)n + 1)newCap *= 2;
					char* nb = (char*)realloc(buf, newCap);
					if (!nb) {
						free(buf);
						parser->error = JSON_ALLOCFAILED;
						return NULL;
					}
					buf = nb;
					cap = newCap;
				}
				for (int i = 0; i < n; i++) {
					buf[len++] = utf8[i];
				}
				continue;
			}
			default:
				free(buf);
				parser->error = JSON_PARSEERROR;
				return NULL;
			}
		}

		// 写入 buf，容量不够就扩容
		if (len + 1 >= cap) {
			cap *= 2;
			char* nb = (char*)realloc(buf, cap);
			if (!nb) {
				free(buf);
				parser->error = JSON_ALLOCFAILED;
				return NULL;
			}
			buf = nb;
		}
		buf[len++] = c;
	}

	// 没找到结束的 '"'
	free(buf);
	parser->error = JSON_UNEXPECTEDEND;
	return NULL;
}

static jsonNode* parseNumber(jsonParser* parser) {
	const char* s = parser->text.data;
	size_t len = parser->text.length;
	size_t start = parser->position;
	size_t i = start;
	bool ok = true;

	// 可选负号
	if (i < len && s[i] == '-') i++;

	// 整数部分：0 或 [1-9][0-9]*
	if (i >= len) {
		ok = false;
	}
	else if (s[i] == '0') {
		i++;
	}
	else if (s[i] >= '1' && s[i] <= '9') {
		i++;
		while (i < len && s[i] >= '0' && s[i] <= '9') i++;
	}
	else {
		ok = false;
	}

	// 可选小数部分：. [0-9]+
	if (ok && i < len && s[i] == '.') {
		i++;
		if (i >= len || s[i] < '0' || s[i] > '9') {
			ok = false;
		}
		else {
			while (i < len && s[i] >= '0' && s[i] <= '9') i++;
		}
	}

	// 可选指数部分：[eE] [+-]? [0-9]+
	if (ok && i < len && (s[i] == 'e' || s[i] == 'E')) {
		i++;
		if (i < len && (s[i] == '+' || s[i] == '-')) i++;
		if (i >= len || s[i] < '0' || s[i] > '9') {
			ok = false;
		}
		else {
			while (i < len && s[i] >= '0' && s[i] <= '9') i++;
		}
	}

	if (!ok) {
		parser->error = JSON_PARSEERROR;
		return NULL;
	}

	parser->position = i;
	size_t numLen = i - start;

	char* buf = (char*)malloc(numLen + 1);
	if (!buf) {
		parser->error = JSON_ALLOCFAILED;
		return NULL;
	}
	memcpy(buf, s + start, numLen);
	buf[numLen] = '\0';

	char* endptr = NULL;
	double num = strtod(buf, &endptr);
	if (endptr != buf + numLen) {
		free(buf);
		parser->error = JSON_PARSEERROR;
		return NULL;
	}
	free(buf);

	jsonNode* node = newNumberNode(num);
	if (!node) {
		parser->error = JSON_ALLOCFAILED;
		return NULL;
	}
	return node;
}

static jsonNode* parseLiteral(jsonParser* parser) {
	char c = parser->text.data[parser->position];

	if (c == 't') {
		if (parser->position + 4 <= parser->text.length &&
			memcmp(parser->text.data + parser->position, "true", 4) == 0) {
			parser->position += 4;
			return newBoolNode(true);
		}
	}
	else if (c == 'f') {
		if (parser->position + 5 <= parser->text.length &&
			memcmp(parser->text.data + parser->position, "false", 5) == 0) {
			parser->position += 5;
			return newBoolNode(false);
		}
	}
	else if (c == 'n') {
		if (parser->position + 4 <= parser->text.length &&
			memcmp(parser->text.data + parser->position, "null", 4) == 0) {
			parser->position += 4;
			return newNullNode();
		}
	}

	parser->error = JSON_PARSEERROR;
	return NULL;
}

static bool readHex4(jsonParser* parser, uint32_t* out) {
	if (parser->position + 4 > parser->text.length) return false;

	uint32_t v = 0;
	for (int i = 0; i < 4; i++) {
		char c = parser->text.data[parser->position + i];
		int h;
		if (c >= '0' && c <= '9') h = c - '0';
		else if (c >= 'a' && c <= 'f') h = c - 'a' + 10;
		else if (c >= 'A' && c <= 'F') h = c - 'A' + 10;
		else return false;
		v = (v << 4) | (uint32_t)h;
	}
	parser->position += 4;
	*out = v;
	return true;
}

/* ============================================================
 * 节点构造函数
 * ============================================================ */

static jsonNode* newNullNode(void) {
	jsonNode* node = (jsonNode*)malloc(sizeof(jsonNode));
	if (!node) return NULL;
	node->type = JSON_NULL;
	return node;
}

static jsonNode* newBoolNode(bool b) {
	jsonNode* node = (jsonNode*)malloc(sizeof(jsonNode));
	if (!node) return NULL;
	node->type = JSON_BOOL;
	node->content.boolean = b;
	return node;
}

static jsonNode* newNumberNode(double n) {
	jsonNode* node = (jsonNode*)malloc(sizeof(jsonNode));
	if (!node) return NULL;
	node->type = JSON_NUMBER;
	node->content.number = n;
	return node;
}

static jsonNode* newStringNode(const char* s, size_t len) {
	jsonNode* node = (jsonNode*)malloc(sizeof(jsonNode));
	if (!node) return NULL;

	char* data = (char*)malloc(len + 1);
	if (!data) {
		free(node);
		return NULL;
	}

	if (s && len > 0) {
		memcpy(data, s, len);
	}
	data[len] = '\0';

	node->type = JSON_STRING;
	node->content.string.data = data;
	node->content.string.length = len;
	return node;
}

static jsonNode* newArrayNode(void) {
	jsonNode* node = (jsonNode*)malloc(sizeof(jsonNode));
	if (!node) return NULL;
	node->type = JSON_ARRAY;
	node->content.array.items = NULL;
	node->content.array.length = 0;
	node->content.array.capacity = 0;
	return node;
}

static jsonNode* newObjectNode(void) {
	jsonNode* node = (jsonNode*)malloc(sizeof(jsonNode));
	if (!node) return NULL;
	node->type = JSON_OBJECT;
	node->content.object.members = NULL;
	node->content.object.length = 0;
	node->content.object.capacity = 0;
	return node;
}

/* ============================================================
 * 往数组 / 对象添加元素
 * ============================================================ */

static bool addItem(jsonNode* array, jsonNode* value) {
	if (!array || array->type != JSON_ARRAY) return false;

	jsonArray* arr = &array->content.array;

	if (arr->length == arr->capacity) {
		size_t newCap = (arr->capacity == 0) ? 8 : arr->capacity * 2;

		// 防止容量翻倍溢出
		if (newCap < arr->capacity) return false;

		jsonNode** newItems = (jsonNode**)realloc(arr->items, newCap * sizeof(jsonNode*));
		if (!newItems) return false;

		arr->items = newItems;
		arr->capacity = newCap;
	}

	arr->items[arr->length++] = value;
	return true;
}

static bool addMember(jsonNode* object, jsonString key, jsonNode* value) {
	if (!object || object->type != JSON_OBJECT) return false;

	jsonObject* obj = &object->content.object;

	if (obj->length == obj->capacity) {
		size_t newCap = (obj->capacity == 0) ? 8 : obj->capacity * 2;

		if (newCap < obj->capacity) return false;

		jsonMember** newMembers = (jsonMember**)realloc(obj->members, newCap * sizeof(jsonMember*));
		if (!newMembers) return false;

		obj->members = newMembers;
		obj->capacity = newCap;
	}

	jsonMember* member = (jsonMember*)malloc(sizeof(jsonMember));
	if (!member) return false;

	member->key = key;       // 接管 key 的所有权
	member->value = value;   // 接管 value 的所有权

	obj->members[obj->length++] = member;
	return true;
}

jsonParser* jsonParserCreate(const char* filepath) {
	// Check if the filepath is NULL
	if (!filepath) {
		jsonParser* parser = generateParser(NULL, 0, 0, JSON_NULLPATH, EINVAL);
		return parser;
	}

	// Create a trimmed copy of the filepath
	size_t pathLen = strlen(filepath);
	jsonString trimmedPath = {
		.length = pathLen,
		.data = (char*)malloc(pathLen + 1)
	};

	// Check if the memory allocation was successful
	if (!trimmedPath.data) {
		jsonParser* parser = generateParser(NULL, 0, 0, JSON_ALLOCFAILED, EINVAL);
		return parser;
	}

	// Copy the filepath to the trimmedPath and null-terminate it
	memcpy(trimmedPath.data, filepath, trimmedPath.length + 1);

	// Trim the whitespace from the trimmedPath
	int trimStatus = trim(&trimmedPath);
	if (trimStatus != JSON_SUCCESSFULOP) {
		jsonParser* parser = generateParser(NULL, 0, 0, trimStatus, 0);
		free(trimmedPath.data);
		return parser;
	}

	FILE* file = fopen(trimmedPath.data, "rb");
	free(trimmedPath.data);
	if (!file)return generateParser(NULL, 0, 0, JSON_OPENFILEFAILED, errno);

	if (jsonFseek64(file, 0, SEEK_END) != 0) {
		int savedError = errno;
		fclose(file);
		return generateParser(NULL, 0, 0, JSON_SEEKFAILED, savedError);
	}

	int64_t fileSize = jsonFtell64(file);
	if (fileSize < 0) {
		int savedError = errno;
		fclose(file);
		return generateParser(NULL, 0, 0, JSON_TELLFAILED, savedError);
	}
	if ((uint64_t)fileSize >= SIZE_MAX) {
		fclose(file);
		return generateParser(NULL, 0, 0, JSON_TOOLARGE, ENOMEM);
	}
	if (jsonFseek64(file, 0, SEEK_SET) != 0) {
		int savedError = errno;
		fclose(file);
		return generateParser(NULL, 0, 0, JSON_SEEKFAILED, savedError);
	}

	char* text = (char*)malloc((size_t)fileSize + 1);
	if (!text) {
		fclose(file);
		return generateParser(NULL, 0, 0, JSON_ALLOCFAILED, 0);
	}

	size_t read = fread(text, 1, (size_t)fileSize, file);
	if (read != (size_t)fileSize) {
		int savedErrno = errno;
		free(text);
		fclose(file);
		return generateParser(NULL, 0, 0, JSON_READFAILED, savedErrno);
	}
	fclose(file);
	text[fileSize] = '\0';

	jsonParser* parser = generateParser(text, (size_t)fileSize, 0, JSON_SUCCESSFULOP, 0);
	if (!parser)free(text);
	return parser;
}

void jsonNodeDestroy(jsonNode* node) {
	if (!node) return;
	switch (node->type) {
	case JSON_STRING:
		free(node->content.string.data);
		break;
	case JSON_ARRAY:
		for (size_t i = 0; i < node->content.array.length; i++) {
			jsonNodeDestroy(node->content.array.items[i]);
		}
		free(node->content.array.items);
		break;
	case JSON_OBJECT:
		for (size_t i = 0; i < node->content.object.length; i++) {
			jsonMember* m = node->content.object.members[i];
			free(m->key.data);
			jsonNodeDestroy(m->value);
			free(m);
		}
		free(node->content.object.members);
		break;
	default:
		break;
	}
	free(node);
}

int jsonGetParserStatus(jsonParser* parser) {
	if (!parser)return JSON_NULLPARSER;
	return (int)parser->error;
}

jsonNode* jsonParse(jsonParser* parser) {
	if (!parser)return NULL;

	if (parser->error != JSON_SUCCESSFULOP)return NULL;

	skipWhitespace(parser);

	jsonNode* root = parseValue(parser);
	if (!root)return NULL;

	skipWhitespace(parser);

	if (parser->position != parser->text.length) {
		parser->error = JSON_PARSEERROR;
		jsonNodeDestroy(root);
		return NULL;
	}

	return root;
}