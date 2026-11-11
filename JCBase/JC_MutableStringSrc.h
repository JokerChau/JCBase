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
	JC_MS__NULL_FOLLOWER,					// append 的 src 为 NULL
	JC_MS__NULL_FOLLOWER_BUFFER,			// append 的 src 的 buffer 为 NULL
	JC_MS__NULL_RECEIVER,					// 接受器为 NULL

	JC_MS__NULL_INT,
	JC_MS__OVERFLOW_INT,
	JC_MS__NOT_INT,

	JC_MS__NULL_LONG,
	JC_MS__OVERFLOW_LONG,
	JC_MS__NOT_LONG,

	JC_MS__NULL_LONGLONG,
	JC_MS__OVERFLOW_LONGLONG,
	JC_MS__NOT_LONGLONG,

	JC_MS__NULL_FLOAT,
	JC_MS__OVERFLOW_FLOAT,
	JC_MS__NOT_FLOAT,

	JC_MS__NULL_DOUBLE,
	JC_MS__OVERFLOW_DOUBLE,
	JC_MS__NOT_DOUBLE,
} JC_MutableStringStatus;