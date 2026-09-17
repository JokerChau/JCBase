#pragma once

typedef enum stringCommonStatus {
	// 实例自身状态
	NULLSTRING = -1,			// String 实例为 NULL
	NULLSB = -1,				// StringBuilder 实例为 NULL
	AVAILABLE = 0,				// 实例可用
	NULLBUFFER = 1,				// 实例的 buffer 为 NULL
	EMPTYBUFFER,				// buffer 为空（未分配）

	EMPTYSTRING,				// String 空串
	EMPTYSB,					// StringBuilder 空串
	BLANKSTRING,				// String 全空白
	BLANKSB,					// StringBuilder 全空白

	// 操作错误状态
	FAILEDTOCOMBINE,			// 内容结合失败
	WRONGSTRINGSTATUS,			// String 状态非可用
	WRONGSBSTATUS,				// StringBuilder 状态非可用
	FAILEDTOREALLOC,			// 重分配内存失败
	FAILEDTOALLOC,				// 内存操作失败

	// 转换状态
	NULLINT,
	OVERFLOWINT,
	NOTINT,
	SUCCEEDTOINT,

	NULLLONG,
	OVERFLOWLONG,
	NOTLONG,
	SUCCEEDTOLONG,

	NULLLONGLONG,
	OVERFLOWLONGLONG,
	NOTLONGLONG,
	SUCCEEDTOLONGLONG,

	NULLFLOAT,
	OVERFLOWFLOAT,
	NOTFLOAT,
	SUCCEEDTOFLOAT,

	NULLDOUBLE,
	OVERFLOWDOUBLE,
	NOTDOUBLE,
	SUCCEEDTODOUBLE,

	// 字符数组状态
	NULLCHAR,					// 无效的待接受 char*
	NULLARRAY,					// 输出数组指针为 NULL
	NULLCOUNT,					// 计数器指针为 NULL
	NULLTARGET,					// target 指针为 NULL

	// 检索状态
	IDXOUTBOUNDS,				// 索引越界
	SUCCESSFULIDX,				// 索引有效
	SUCCEEDTOTRUNCATESTRING,	// 成功截断 String
	SUCCEEDTOTRUNCATESB,		// 成功截断 StringBuilder
	SUCCEEDTOFINDCH,			// 成功通过 ch 找到索引
	WITHOUTCH,					// 没有通过 ch 找到索引
	INVALIDSTART,				// 起始点索引无效
	INVALIDEND,					// 结束点索引无效
	INVALIDIDX,					// 索引无效
	INVALIDREPLACEMENT,			// 替换内容无效

	// 操作状态
	SUCCESSFULOP,				// 操作成功
	PARTIALSPLIT,				// 分割部分成功（数组中有 NULL）
	NOTARGETMATCHED,			// target 未匹配到
} stringCommonStatus;