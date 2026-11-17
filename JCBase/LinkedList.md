# LinkedList
## 其他语言md
### English
[LinkedList.md English version](LinkedList_EN.md)
## 接口介绍

> jc_LinkedList jcll_inner_new(size_t typeSize)
```text
此为内部函数，返回一个带状态的空链表实例（带有头哨兵节点），需要传入元素类型的大小
注意：若 typeSize 为 0，返回的实例状态为 JC_LL__INVALID_TYPE_SIZE；若内存分配失败，状态可能为 JC_LL__NULL_HEADER；返回的实例需通过 jcll_delete 释放
```
> jcll_newOf(type)
```text
此为推荐宏，创建一个默认的 LinkedList 实例
注意：利用 _Generic 在编译期拦截 void 类型，并自动计算 sizeof(type)
```
> void jcll_delete(jc_LinkedList list)
```text
释放传入的链表实例及其内部所有节点和节点数据
注意：传入 NULL 不会崩溃；仅释放链表自身分配的内存，不负责释放存放在容器内的指针所指向的外部深层内存（浅拷贝语义）
```
> size_t jcll_getLength(const JC_LinkedList* const list)
```text
返回当前链表中的元素数量（count）
注意：传入 NULL 时返回 0
```
> int jcll_getStatus(const JC_LinkedList* const list)
```text
返回实例当前的状态码
注意：传入 NULL 返回 JC_LL__NULL
```
> int jcll_clear(jc_LinkedList list)
```text
清空链表中的所有数据节点，重置为空链表状态
注意：此操作是幂等的，不论链表当前是否为空，都返回成功；释放所有数据节点及其内部的 data 内存，但保留头哨兵节点结构
```
> const char* jcll_statusToCharArray(int status)
```text
返回状态码对应的可读字符串描述
注意：返回的是静态字符串，调用者无需也不应释放
```
> int jcll_insertAt(jc_LinkedList list, const void* const content, size_t index)
```text
在指定索引 index 处插入一个元素
注意：允许 index 等于当前元素数量（即尾插）；index 大于 count 时返回 JC_LL__INDEX_OUT_OF_BOUNDS；如果 content 为 NULL，会在该位置插入一个 data 为 NULL 的节点；时间复杂度为 O(N)（底层使用了双向逼近优化）
```
> int jcll_pushBack(jc_LinkedList list, const void* const content)
```text
在链表尾部追加一个元素
注意：等价于 jcll_insertAt(list, content, list->count)
```
> int jcll_pushFront(jc_LinkedList list, const void* const content)
```text
在链表头部插入一个元素
注意：等价于 jcll_insertAt(list, content, 0)
```
> int jcll_removeAt(jc_LinkedList list, size_t index)
```text
移除索引 index 处的元素
注意：index 越界返回 JC_LL__INDEX_OUT_OF_BOUNDS；链表为空时返回 JC_LL__ALREADY_EMPTY；时间复杂度为 O(N)
```
> int jcll_popBack(jc_LinkedList list)
```text
移除链表尾部的元素
注意：链表为空时返回 JC_LL__ALREADY_EMPTY
```
> int jcll_popFront(jc_LinkedList list)
```text
移除链表头部的元素
注意：链表为空时返回 JC_LL__ALREADY_EMPTY
```
> int jcll_getContentAt(const JC_LinkedList* const list, size_t index, void* outContent)
```text
获取索引为 index 的元素，并将其内容拷贝到 outContent 指向的内存中
注意：调用方必须保证 outContent 指向的内存大小至少为 typeSize；如果该节点存的是 NULL 数据，会将 outContent 的内存清零（memset）并返回成功；index 越界返回 JC_LL__INDEX_OUT_OF_BOUNDS
```
> int jcms_setContentAt(jc_LinkedList list, size_t index, const void* const content)
```text
将索引为 index 的元素内容替换为 content 指向的内容
注意：如果 content 为 NULL，会释放该节点原有的数据内存并将节点 data 置为 NULL；如果原节点 data 为 NULL，会先分配内存再拷贝；index 越界返回 JC_LL__INDEX_OUT_OF_BOUNDS
```