#pragma once

typedef enum JC_LinkedListStatus {
	JC_LL__NULL = -2,               // 实例为NULL
	JC_LL__AVAILABLE = 100,         // 实例可用
	JC_LL__INVALID_TYPE_SIZE,       // 不可用的类型大小
	JC_LL__NULL_HEADER,             // 头结点为NULL
	JC_LL__ALREADY_EMPTY,			// 实例本身没有数据结点

	JC_LL__SUCCESSFUL_OP,           // 操作成功
	JC_LL__ALLOCATION_FAILED,       // 内存分配失败

	JC_LL__INDEX_OUT_OF_BOUNDS,     // 传入的索引越界
	JC_LL__NULL_RECEIVER,			// 接收器为NULL
} JC_LinkedListStatus;