#pragma once

typedef enum alStatus {
	// ArrayList
	AL_NULL = -2,					// NULL AL实例
	AL_AVAILABLE = 100,				// 可用实例
	AL_INVALIDTYPESIZE,				// 非法数据类型（大小）
	AL_ALREADYEMPTY,				// 实例元素本就为0

	// 操作状态
	AL_SUCCESSFULOP,				// 操作成功
	AL_WRONGSTATUS,				// 未知实例状态（非可用），若需获取状态请调用getAlStatus
	AL_REALLOCFAIL,				// 重分配内存失败

	// 参数状态
	AL_NULLELEMENT,				// 元素指向NULL
	AL_IDXOUTBOUNDS,				// 所传索引越界
	AL_CAPACITYLESSTHANMIN,			// 所传容量过小
	AL_REALLOCSIZEOUTBOUNDS,		// realloc函数所传参数过大
}alStatus;