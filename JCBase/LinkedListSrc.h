#pragma once

typedef enum llStatus {
	LL_NULL = -2,					// 实例为NULL
	LL_AVAILABLE = 100,				// 实例可用
	LL_NULLHEADER,					// 头结点为NULL
	LL_HEADERDELETING,				// 头结点不可删除
	LL_ISHEADER,					// 该结点为头结点
	LL_NULLNODE,					// 结点为NULL
	LL_NULLNEWNODE,					// 新申请的结点为NULL
	LL_NULLDATA,					// 结点数据为NULL
	LL_INVALIDTYPESIZE,				// 不可用的类型大小
	LL_ALREADYEMPTY,				// 链表早已没有数据结点
	LL_SAMELL,						// 相同链表

	LL_NULLCONTENT,					// 传入的content是NULL
	LL_INDEXOUTOFBOUNDS,			// 传入的索引越界
	LL_NULLRECEIVER,				// 传入的接收器为NULL
	LL_EMPTYRANGE,					// 传入的区间为空区间

	LL_SUCCESSFULOP,				// 操作成功
}llStatus;