#pragma once

// naming rule
// NULL_<what>/<operation>_FAILED
typedef enum JC_ArrayListStatus {
	// AL 实例状态
	JC_AL__NULL = -2,						// NULL AL实例
	JC_AL__AVAILABLE = 100,					// 可用实例
	JC_AL__INVALID_TYPE_SIZE,				// 非法数据类型（大小）
	JC_AL__ALREADY_EMPTY,					// 实例元素数量本就为0
	// 操作状态
	JC_AL__SUCCESSFUL_OP,					// 操作成功
	JC_AL__REALLOCATION_FAILED,				// 重分配内存失败
	JC_AL__REALLOC_SIZE_MAX,				// 已经到达理论最大容量，无法再涨
	// 参数状态
	JC_AL__CAPACITY_LESS_THAN_MIN,			// 新的最大容量小于当前元素数量
	JC_AL__COUNT_TOO_LARGE,					// 新的最大容量*类型大小
	JC_AL__NULL_RECEIVER,					// 接收器指针为 NULL
	JC_AL__INDEX_OUT_OF_BOUNDS,				// 索引超限
	JC_AL__NULL_CONTENT,					// content 参数为 NULL
} JC_ArrayListStatus;