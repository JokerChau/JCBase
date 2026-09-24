#pragma once

typedef enum hashMapStatus {
	HM_NULL = -2,					// NULL HM 句柄
	HM_AVAILABLE = 100,				// 可用 HM
	HM_INVALIDSIZE,					// keySize 或 valueSize 为 0
	HM_WRONGSTATUS,					// 状态不可用
	HM_NULLKEY,						// key 为 NULL
	HM_NULLVALUE,					// value 为 NULL
	HM_NULLOUTVALUE,				// 输出参数为 NULL
	HM_KEYNOTFOUND,					// 查找/删除时 key 不存在

	HM_EXPANDFAILED,				// 数据已插入，但扩容失败
	HM_SHRINKFAILED,				// 数据已删除，但缩容失败
	HM_IDENTIFIEREXHAUSTED,			// identifier 已用尽，无法再创建

	HM_REALLOCFAIL,					// 重分配内存失败
	HM_MALLOCFAIL,					// 分配内存失败
	HM_SUCCESSFULOP,				// 操作成功
} hashMapStatus;