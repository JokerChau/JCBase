#pragma once

typedef enum Status {
	// String
	// 实例自身状态
	NULLSTRING = -1,				// string实例为NULL
	AVAILABLE = 0,					// string实例可用
	NULLBUFFER = 1,					// string的buffer为NULL
	EMPTYSTRING = 2,				// 实例为NULL/buffer为NULL/buffer为\0
	BLANKSTRING = 3,				// 实例为NULL/buffer为NULL/buffer为空白

	// 操作错误状态
	FAILEDTOCOMBINE = 4,			// 内容结合失败
	WRONGSTRINGSTATUS = 5,			// 实例状态非可用
	FAILEDTOREALLOC = 6,			// 重分配内存失败
	FAILEDTOALLOC = 7,				// 内存操作失败

	// 转换状态
	NULLINT = 8,
	OVERFLOWINT = 9,
	NOTINT = 10,
	SUCCEEDTOINT = 11,

	NULLLONG = 12,
	OVERFLOWLONG = 13,
	NOTLONG = 14,
	SUCCEEDTOLONG = 15,

	NULLLONGLONG = 16,
	OVERFLOWLONGLONG = 17,
	NOTLONGLONG = 18,
	SUCCEEDTOLONGLONG = 19,

	NULLFLOAT = 20,
	OVERFLOWFLOAT = 21,
	NOTFLOAT = 22,
	SUCCEEDTOFLOAT = 23,

	NULLDOUBLE = 24,
	OVERFLOWDOUBLE = 25,
	NOTDOUBLE = 26,
	SUCCEEDTODOUBLE = 27,

	// 字符数组状态
	NULLCHAR = 28,					// 无效的待接受char*

	// 检索状态
	IDXOUTBOUNDS = 29,				// 索引越界
	SUCCESSFULIDX = 30,				// 索引有效
	SUCCEEDTOTRUNCATESTRING = 31,	// 成功修改索引处内容
	SUCCEEDTOFINDCH = 32,			// 成功通过ch找到索引
	WITHOUTCH = 33,					// 没有通过ch找到索引
	INVALIDSTART = 34,				// 起始点索引无效
	INVALIDEND = 35,
	INVALIDIDX = 36,
	INVALIDREPLACEMENT = 37,


	// ArrayList
	// ArrayList状态
	NULLAL = -2,
	AVAILABLEAL = 100,
	INVALIDALTYPESIZE = 101,
	WRONGALSTATUS = 102,
	NULLALELEMENT = 103,
	ALLOCALFAIL = 104,
}Status;