#include "JC_ArrayList.h"
#include "JC_ArrayListStructures.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// enable
// 接管 data
static jc_ArrayList wrapAl(void* data, size_t count,
	size_t maxCount, size_t typeSize,
	int status, unsigned int flags) {
	jc_ArrayList list = (jc_ArrayList)malloc(sizeof(JC_ArrayList));
	if (!list)return NULL;
	list->data = data;
	list->count = count;
	list->maxCount = maxCount;
	list->typeSize = typeSize;
	list->status = status;
	list->flags = flags;
	return list;
}

static int alGrow(JC_ArrayList* list) {
	if (list->count >= list->maxCount) {
		// 设置临时容量，如果最大容量为0则设为4，且每次加4
		size_t newCap;
		size_t newMaxCap = SIZE_MAX / list->typeSize;
		if (list->maxCount == 0) {
			newCap = (newMaxCap >= 8) ? 8 : newMaxCap;
		}
		else if (list->maxCount >= newMaxCap) {
			return JC_AL__REALLOC_SIZE_MAX;
		}
		else if (list->maxCount > newMaxCap / 2) {
			newCap = newMaxCap;
		}
		else if (list->maxCount >= 1000) {
			// 1.5 倍扩容
			newCap = list->maxCount + (list->maxCount >> 1);
			if (newCap > newMaxCap) newCap = newMaxCap; // 防溢出
		}
		else {
			// 2 倍扩容
			newCap = list->maxCount * 2;
		}
		// 重分配一个数据指针，大小为临时容量*类型大小
		void* newData = realloc(list->data, newCap * list->typeSize);
		if (!newData)return JC_AL__REALLOCATION_FAILED;
		// 分配成功，更新数据
		list->data = newData;
		list->maxCount = newCap;
		return JC_AL__SUCCESSFUL_OP;
	}
	return JC_AL__SUCCESSFUL_OP;
}

// 在 delete / pop 等减少元素的函数末尾调用
static int alShrink(JC_ArrayList* list) {
	// 1. 没达到触发条件 (即使用率 >= 25%)，不缩
	if (list->maxCount <= 8) return JC_AL__SUCCESSFUL_OP; // 底线：不低于初始容量
	if (list->count >= list->maxCount / 4) return JC_AL__SUCCESSFUL_OP;

	// 2. 计算新容量（缩一半）
	size_t newCap = list->maxCount / 2;
	if (newCap < 8) newCap = 8; // 再次确保底线

	// 3. realloc
	void* newData = realloc(list->data, newCap * list->typeSize);
	// ... (注意：realloc 缩容虽然很少失败，但还是要处理失败情况)

	// 成功则更新
	if (newData) { // 缩容有时在某些系统上可能返回 NULL 但原数据有效？标准库对缩容的处理可能有所不同
		list->data = newData;
		list->maxCount = newCap;
	}
	return JC_AL__SUCCESSFUL_OP;
}

// enable
jc_ArrayList jcal_inner_new(size_t sizeType, unsigned int flags) {
	int status = JC_AL__AVAILABLE;
	if (!sizeType) {
		status = JC_AL__INVALID_TYPE_SIZE;
	}
	return wrapAl(NULL, 0, 0, sizeType, status, flags);
}

unsigned int jcal_getFlags(const jc_ArrayList list) {
	if (!list) return 0;
	return list->flags;
}

void jcal_setFlags(jc_ArrayList list, unsigned int flags) {
	if (!list) return;
	list->flags = flags;
}

// enable
void jcal_delete(jc_ArrayList list) {
	if (!list)return;
	if (list->data)free(list->data);
	free(list);
}

// enable
size_t jcal_getLength(const jc_ArrayList list) {
	if (!list)return 0;
	if (!list->data)return 0;
	return list->count;
}

// enable
size_t jcal_getMaxLength(const jc_ArrayList list) {
	if (!list)return 0;
	return list->maxCount;
}

// enable
int jcal_getStatus(const jc_ArrayList list) {
	if (!list)return JC_AL__NULL;
	return list->status;
}

// enable
// 往大设可以，往小不能小于元素数量
int jcal_setMaxLength(jc_ArrayList list, size_t count) {
	if (!list)return JC_AL__NULL;
	if (list->status != JC_AL__AVAILABLE)return list->status;
	// 如果新的最大长度小于当前元素数量，返回错误
	if (count < list->count)return JC_AL__CAPACITY_LESS_THAN_MIN;
	// 如果新的最大长度等于当前最大长度，直接返回成功
	if (count == list->maxCount)return JC_AL__SUCCESSFUL_OP;
	// 其余情况均为重新分配内存

	if (!count) {
		free(list->data);
		list->data = NULL;
		list->maxCount = count;
		return JC_AL__SUCCESSFUL_OP;
	}

	// 如果新的最大长度*类型大小大于SIZE_MAX，返回错误
	if (count > SIZE_MAX / list->typeSize)return JC_AL__COUNT_TOO_LARGE;

	void* newData = realloc(list->data, count * list->typeSize);
	if (!newData)return JC_AL__REALLOCATION_FAILED;
	list->data = newData;
	list->maxCount = count;
	return JC_AL__SUCCESSFUL_OP;
}

// enable
// 只清空记录，不释放内存
int jcal_clear(jc_ArrayList list) {
	if (!list)return JC_AL__NULL;
	if (list->status != JC_AL__AVAILABLE)return list->status;
	list->count = 0;
	return JC_AL__SUCCESSFUL_OP;
}

// enable
// 将内存降至刚好合适
int jcal_shrinkToFit(const jc_ArrayList list) {
	if (!list)return JC_AL__NULL;
	if (list->status != JC_AL__AVAILABLE)return list->status;

	if (list->count == list->maxCount)return JC_AL__SUCCESSFUL_OP;

	if (!list->count) {
		free(list->data);
		list->data = NULL;
		list->maxCount = 0;
		return JC_AL__SUCCESSFUL_OP;
	}

	if (list->count > SIZE_MAX / list->typeSize) return JC_AL__COUNT_TOO_LARGE;

	void* newData = realloc(list->data, list->count * list->typeSize);
	if (!newData)return JC_AL__REALLOCATION_FAILED;
	list->data = newData;
	list->maxCount = list->count;
	return JC_AL__SUCCESSFUL_OP;
}

// enable
// 预分配容量，如果 capacity 比当前 maxCount 小，则不执行任何操作
int jcal_reserve(jc_ArrayList list, size_t capacity) {
	if (!list) return JC_AL__NULL;
	if (list->status != JC_AL__AVAILABLE) return list->status;

	// 如果请求的容量比当前容量小或相等，直接返回成功（不做任何操作）
	if (capacity <= list->maxCount) return JC_AL__SUCCESSFUL_OP;

	// 边界检查：防止乘法溢出
	if (capacity > SIZE_MAX / list->typeSize) return JC_AL__COUNT_TOO_LARGE;

	void* newData = realloc(list->data, capacity * list->typeSize);
	if (!newData) return JC_AL__REALLOCATION_FAILED;

	list->data = newData;
	list->maxCount = capacity;

	return JC_AL__SUCCESSFUL_OP;
}

// enable
int jcal_getAt(const JC_ArrayList* const list, size_t index, void* ele) {
	if (!list)return JC_AL__NULL;
	if (list->status != JC_AL__AVAILABLE)return list->status;
	if (!ele)return JC_AL__NULL_RECEIVER;
	if (index >= list->count)return JC_AL__INDEX_OUT_OF_BOUNDS;

	char* tempData = (char*)list->data + index * list->typeSize;
	memcpy(ele, tempData, list->typeSize);

	return JC_AL__SUCCESSFUL_OP;
}

int jcal_setAt(JC_ArrayList* const list, size_t index, const void* const content) {
	if (!list)return JC_AL__NULL;
	if (list->status != JC_AL__AVAILABLE)return list->status;
	if (!content)return JC_AL__NULL_CONTENT;
	if (index >= list->count)return JC_AL__INDEX_OUT_OF_BOUNDS;

	memcpy((char*)list->data + index * list->typeSize, content, list->typeSize);

	return JC_AL__SUCCESSFUL_OP;
}

int jcal_pushBack(JC_ArrayList* const list, const void* const content) {
	if (!list)return JC_AL__NULL;
	return jcal_insertAt(list, content, list->count);
}

int jcal_pushFront(JC_ArrayList* const list, const void* const content) {
	if (!list)return JC_AL__NULL;
	return jcal_insertAt(list, content, 0);
}

int jcal_insertAt(JC_ArrayList* const list, const void* const content, size_t index) {
	if (!list)return JC_AL__NULL;
	if (list->status != JC_AL__AVAILABLE)return list->status;
	if (!content)return JC_AL__NULL_CONTENT;
	if (index > list->count)return JC_AL__INDEX_OUT_OF_BOUNDS;

	int tempStatus = alGrow(list);
	if (tempStatus != JC_AL__SUCCESSFUL_OP)return tempStatus;

	if (index == list->count) {
		char* dest = (char*)list->data + list->count * list->typeSize;
		memcpy(dest, content, list->typeSize);
	}
	else {
		char* dest = (char*)list->data + list->typeSize * (index + 1);
		char* src = (char*)list->data + list->typeSize * index;
		size_t n = (list->count - index) * list->typeSize;
		memmove(dest, src, n);
		memcpy((char*)list->data + list->typeSize * index, content, list->typeSize);
	}
	list->count++;
	return JC_AL__SUCCESSFUL_OP;
}

int jcal_popBack(JC_ArrayList* const list) {
	if (!list)return JC_AL__NULL;
	if (list->status != JC_AL__AVAILABLE)return list->status;
	if (!list->count)return JC_AL__ALREADY_EMPTY;
	return jcal_removeAt(list, list->count - 1);
}

int jcal_popFront(JC_ArrayList* const list) {
	if (!list)return JC_AL__NULL;
	if (list->status != JC_AL__AVAILABLE)return list->status;
	if (!list->count)return JC_AL__ALREADY_EMPTY;
	return jcal_removeAt(list, 0);
}

int jcal_removeAt(JC_ArrayList* const list, size_t index) {
	if (!list)return JC_AL__NULL;
	if (list->status != JC_AL__AVAILABLE)return list->status;
	if (!list->count)return JC_AL__ALREADY_EMPTY;
	if (index >= list->count)return JC_AL__INDEX_OUT_OF_BOUNDS;

	if (index < list->count - 1) {
		char* dest = (char*)list->data + list->typeSize * index;
		char* src = (char*)list->data + list->typeSize * (index + 1);
		size_t n = (list->count - index - 1) * list->typeSize;
		memmove(dest, src, n);
	}
	list->count--;

	if (list->flags & JC_AL_FLAG_AUTO_SHRINK)alShrink(list);

	return JC_AL__SUCCESSFUL_OP;
}

const char* jcal_statusToCharArray(int status) {
	switch (status) {
	default: {
		return "unknown status";
	}
	}
}