#pragma once

typedef enum JC_MutableStringStatus {
	JC_MS__NULL = -1,						// mutableString 实例为 NULL
	JC_MS__AVAILABLE = 0,					// 实例可用
	JC_MS__NULL_BUFFER = 1,					// 实例的 buffer 为 NULL
	JC_MS__EMPTY,							// mutableString 空串
	JC_MS__BLANK,							// mutableString 全空白
	JC_MS__INVALID_START,					// 起始点索引无效
	JC_MS__INVALID_END,						// 结束点索引无效
	JC_MS__INVALID_INDEX,					// 索引无效

	JC_MS__SUCCESSFUL_OP,					// 操作成功
	JC_MS__REALLOCATION_FAILED,				// 内存重分配失败
	JC_MS__ALLOCATION_FAILED,				// 内存分配失败
	JC_MS__PARTIAL_SPLIT,					// 分割部分成功（数组中有 NULL）
	JC_MS__LENGTH_OUT_OF_BOUNDS,			// 长度越界
	JC_MS__WITHOUT_CHAR,					// 没有通过 ch 找到索引
	JC_MS__NO_TARGET_MATCHED,				// target 未匹配到

	JC_MS__INDEX_OUT_OF_BOUNDS,				// 索引越界
	JC_MS__INVALID_CHARACTER_FOR_CHANGE,	// 替换字符无效
	JC_MS__NULL_CHAR,						// 无效的待接受 char*
	JC_MS__INVALID_SOURCE,					// 待追加的源字符串无效
	JC_MS__NULL_ARRAY,						// 输出数组指针为 NULL
	JC_MS__NULL_COUNT,						// 计数器指针为 NULL
	JC_MS__NULL_RESULT,						// 结果指针为 NULL
	JC_MS__NULL_TARGET,						// target 指针为 NULL
	JC_MS__INVALID_REPLACEMENT,				// 替换内容无效

	MS_NULLINT,
	MS_OVERFLOWINT,
	MS_NOTINT,
	MS_SUCCEEDTOINT,

	MS_NULLLONG,
	MS_OVERFLOWLONG,
	MS_NOTLONG,
	MS_SUCCEEDTOLONG,

	MS_NULLLONGLONG,
	MS_OVERFLOWLONGLONG,
	MS_NOTLONGLONG,
	MS_SUCCEEDTOLONGLONG,

	MS_NULLFLOAT,
	MS_OVERFLOWFLOAT,
	MS_NOTFLOAT,
	MS_SUCCEEDTOFLOAT,

	MS_NULLDOUBLE,
	MS_OVERFLOWDOUBLE,
	MS_NOTDOUBLE,
	MS_SUCCEEDTODOUBLE,

	MS_NULLCHAR,				// 无效的待接受 char*
	MS_NULLARRAY,				// 输出数组指针为 NULL
	MS_NULLCOUNT,				// 计数器指针为 NULL
	MS_NULLTARGET,				// target 指针为 NULL
	MS_NULLRECEIVER,

	MS_IDXOUTBOUNDS,			// 索引越界
	MS_SUCCESSFULIDX,			// 索引有效
	MS_SUCCEEDTOTRUNCATE,		// 成功截断 mutableString
	MS_SUCCEEDTOFINDCH,			// 成功通过 ch 找到索引
	MS_WITHOUTCH,				// 没有通过 ch 找到索引
	MS_INVALIDSTART,			// 起始点索引无效
	MS_INVALIDEND,				// 结束点索引无效
	MS_INVALIDIDX,				// 索引无效
	MS_INVALIDREPLACEMENT,		// 替换内容无效
	MS_LENOUTOFTYPEMAX,
	MS_INVALIDSRC,
	MS_INVALIDCHFORCHANGE,

	MS_SUCCESSFULOP,			// 操作成功
	MS_PARTIALSPLIT,			// 分割部分成功（数组中有 NULL）
	MS_NOTARGETMATCHED,			// target 未匹配到
} JC_MutableStringStatus;