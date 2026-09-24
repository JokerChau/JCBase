#pragma once

typedef enum stringBuilderStatus {
	SB_NULL = -1,				// StringBuilder 实例为 NULL
	SB_AVAILABLE = 0,			// 实例可用
	SB_NULLBUFFER = 1,			// 实例的 buffer 为 NULL
	SB_EMPTY,					// StringBuilder 空串
	SB_BLANK,					// StringBuilder 全空白

	SB_FAILEDTOCOMBINE,			// 内容结合失败
	SB_WRONGSTATUS,				// StringBuilder 状态非可用
	SB_FAILEDTOREALLOC,			// 重分配内存失败
	SB_FAILEDTOALLOC,			// 内存操作失败

	SB_NULLINT,
	SB_OVERFLOWINT,
	SB_NOTINT,
	SB_SUCCEEDTOINT,

	SB_NULLLONG,
	SB_OVERFLOWLONG,
	SB_NOTLONG,
	SB_SUCCEEDTOLONG,

	SB_NULLLONGLONG,
	SB_OVERFLOWLONGLONG,
	SB_NOTLONGLONG,
	SB_SUCCEEDTOLONGLONG,

	SB_NULLFLOAT,
	SB_OVERFLOWFLOAT,
	SB_NOTFLOAT,
	SB_SUCCEEDTOFLOAT,

	SB_NULLDOUBLE,
	SB_OVERFLOWDOUBLE,
	SB_NOTDOUBLE,
	SB_SUCCEEDTODOUBLE,

	SB_NULLCHAR,				// 无效的待接受 char*
	SB_NULLARRAY,				// 输出数组指针为 NULL
	SB_NULLCOUNT,				// 计数器指针为 NULL
	SB_NULLTARGET,				// target 指针为 NULL

	SB_IDXOUTBOUNDS,			// 索引越界
	SB_SUCCESSFULIDX,			// 索引有效
	SB_SUCCEEDTOTRUNCATE,		// 成功截断 StringBuilder
	SB_SUCCEEDTOFINDCH,			// 成功通过 ch 找到索引
	SB_WITHOUTCH,				// 没有通过 ch 找到索引
	SB_INVALIDSTART,			// 起始点索引无效
	SB_INVALIDEND,				// 结束点索引无效
	SB_INVALIDIDX,				// 索引无效
	SB_INVALIDREPLACEMENT,		// 替换内容无效

	SB_SUCCESSFULOP,			// 操作成功
	SB_PARTIALSPLIT,			// 分割部分成功（数组中有 NULL）
	SB_NOTARGETMATCHED,			// target 未匹配到
}stringBuilderStatus;