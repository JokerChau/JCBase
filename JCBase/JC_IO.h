#pragma once

#include "wheel_ex.h"
#include <stdio.h>
#include "JC_IOSrc.h"

typedef struct JC_String JC_String;
typedef JC_String* jc_String;
typedef struct JC_MutableString JC_MutableString;
typedef JC_MutableString* jc_MutableString;

wheels jc_String jcio_readLineString(FILE* fp);

wheels jc_MutableString jcio_readLineMutableString(FILE* fp);

wheels int jcio_writeLineString(FILE* fp, jc_String str);

wheels int jcio_writeLineMutableString(FILE* fp, jc_MutableString ms);