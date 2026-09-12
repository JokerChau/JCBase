#pragma once

typedef enum alStatus {
	// ArrayList
	NULLAL = -2,					// NULL AL实例
	AVAILABLEAL = 100,				// 可用实例
	INVALIDALTYPESIZE,				// 非法数据类型（大小）
	ALALREADYEMPTY,					// 实例元素本就为0

	// 操作状态
	SUCCESSFULOP,					// 操作成功
	WRONGALSTATUS,					// 未知实例状态（非可用），若需获取状态请调用getAlStatus
	REALLOCALFAIL,					// 重分配内存失败
	MALLOCALCHFAIL,

	// 参数状态
	NULLALELEMENT,					// 元素指向NULL
	ALIDXOUTBOUNDS,					// 所传索引越界
	CAPACITYLESSTHANMIN,			// 所传容量过小
	REALLOCSIZEOUTBOUNDS,			// realloc函数所传参数过大
}alStatus;