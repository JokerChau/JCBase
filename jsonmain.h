#pragma once

#include "wheel_json_ex.h"

typedef enum {
	JSON_SUCCESSFULOP,
	JSON_NULLPATH,
	JSON_NULLSTR,
	JSON_NULLSTRBUF,
	JSON_NULLPARSER,
	JSON_ALLOCFAILED,
	JSON_OPENFILEFAILED,
	JSON_SEEKFAILED,
	JSON_TELLFAILED,
	JSON_TOOLARGE,
	JSON_READFAILED,
	JSON_PARSEERROR,
	JSON_UNEXPECTEDEND,
	JSON_INVALIDTOKEN,
}jsonError;

typedef struct jsonString jsonString;
typedef struct jsonArray jsonArray;
typedef struct jsonObject jsonObject;
typedef struct jsonNode jsonNode;
typedef struct jsonMember jsonMember;
typedef struct jsonParser jsonParser;
typedef struct jsonFuncs jsonFuncs;

wheels jsonParser* jsonParserCreate(const char* filepath);

wheels void jsonNodeDestroy(jsonNode* node);

wheels int jsonGetParserStatus(jsonParser* parser);

wheels jsonNode* jsonParse(jsonParser* parser);