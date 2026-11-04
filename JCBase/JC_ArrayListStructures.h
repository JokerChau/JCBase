#pragma once

#include <stddef.h>

struct JC_ArrayList {
	void* data;				// 数据内容
	size_t count;			// 当前数量
	size_t maxCount;		// 最大容量
	size_t typeSize;		// 类型大小
	int status;				// 当前AL状态
};