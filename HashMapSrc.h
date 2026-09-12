#pragma once

typedef enum hashMapStatus {
	NULLHM = -2,					// NULL HM 句柄
	AVAILABLEHM = 100,				// 可用 HM
	INVALIDHMSIZE,					// keySize 或 valueSize 为 0
	WRONGHMSTATUS,					// 状态不可用
	NULLHMKEY,						// key 为 NULL
	NULLHMVALUE,					// value 为 NULL
	NULLHMOUTVALUE,					// 输出参数为 NULL
	HASHMAPKEYNOTFOUND,				// 查找/删除时 key 不存在

	EXPANDFAILED,					// 数据已插入，但扩容失败
	SHRINKFAILED,					// 数据已删除，但缩容失败
	IDENTIFIEREXHAUSTED,			// identifier 已用尽，无法再创建

	REALLOCFAIL,					// 重分配内存失败
	MALLOCFAIL,						// 分配内存失败
	SUCCESSFULOP,					// 操作成功
} hashMapStatus;