# ArrayList
## 其他语言md
### English
[ArrayList.md English version](ArrayList_EN.md)
## 接口介绍

> jc_ArrayList jcal_inner_new(size_t sizeType, unsigned int flags)
```text
此为内部函数，返回一个带状态的空 ArrayList 实例，需要传入元素类型的大小和实例标签（flags）
注意：若 sizeType 为 0，返回的实例状态为 JC_AL__INVALID_TYPE_SIZE；返回的实例需通过 jcal_delete 释放
```
> jcal_newOf(type)
```text
此为推荐宏，创建一个默认标签（flags = 0）的 ArrayList 实例
注意：利用 _Generic 在编译期拦截 void 类型，并自动计算 sizeof(type)
```
> jcal_newOfWithFlags(type, flags)
```text
此为推荐宏，创建一个带指定标签的 ArrayList 实例
注意：可传入 JC_AL_FLAG_AUTO_SHRINK 开启自动缩容
```
> unsigned int jcal_getFlags(const jc_ArrayList list)
```text
获取实例当前的标签（位掩码）
注意：传入 NULL 返回 0
```
> void jcal_setFlags(jc_ArrayList list, unsigned int flags)
```text
设置实例的标签（位掩码）
注意：传入 NULL 不会崩溃；可通过 JC_AL_FLAG_AUTO_SHRINK 动态开关自动缩容
```
> void jcal_delete(jc_ArrayList list)
```text
释放传入的 ArrayList 实例及其内部缓冲区
注意：传入 NULL 不会崩溃；仅释放容器，不负责释放存放在容器内的指针所指向的外部内存（浅释放）
```
> size_t jcal_getLength(const jc_ArrayList list)
```text
返回当前元素数量（count）
注意：传入 NULL 或内部 data 为 NULL 时返回 0
```
> size_t jcal_getMaxLength(const jc_ArrayList list)
```text
返回当前最大容量（maxCount）
注意：传入 NULL 返回 0
```
> int jcal_getStatus(const jc_ArrayList list)
```text
返回实例当前的状态码
注意：传入 NULL 返回 JC_AL__NULL
```
> int jcal_setMaxLength(jc_ArrayList list, size_t count)
```text
强制设置最大容量为 count
注意：若 count 小于当前元素数量，返回 JC_AL__CAPACITY_LESS_THAN_MIN；若 count 为 0，会释放底层内存；此操作可能导致缩容
```
> int jcal_reserve(jc_ArrayList list, size_t capacity)
```text
预分配容量（仅扩不缩），用于性能优化
注意：若 capacity 小于等于当前容量，直接返回成功且不做任何操作；返回成功不代表真的分配了内存，只代表容量已满足
```
> int jcal_clear(jc_ArrayList list)
```text
清空实例中的所有元素（count = 0）
注意：此操作不会释放底层内存，容量（maxCount）保持不变
```
> int jcal_shrinkToFit(const jc_ArrayList list)
```text
将底层内存缩小至刚好容纳当前元素（maxCount = count）
注意：若当前元素为 0，会释放底层内存；返回成功不代表一定会缩容（realloc 可能返回原指针）
```
> int jcal_getAt(const JC_ArrayList* const list, size_t index, void* ele)
```text
获取索引为 index 的元素，并将其内容拷贝到 ele 指向的内存中
注意：调用方必须保证 ele 指向的内存大小至少为 typeSize；index 越界返回 JC_AL__INDEX_OUT_OF_BOUNDS
```
> int jcal_setAt(JC_ArrayList* const list, size_t index, const void* const content)
```text
将索引为 index 的元素内容替换为 content 指向的内容
注意：content 不能为 NULL；index 越界返回 JC_AL__INDEX_OUT_OF_BOUNDS
```
> int jcal_pushBack(JC_ArrayList* const list, const void* const content)
```text
在尾部追加一个元素
注意：内部会自动触发扩容；content 不能为 NULL
```
> int jcal_pushFront(JC_ArrayList* const list, const void* const content)
```text
在头部插入一个元素
注意：时间复杂度为 O(N)，因为需要 memmove 搬移所有元素；content 不能为 NULL
```
> int jcal_insertAt(JC_ArrayList* const list, const void* const content, size_t index)
```text
在指定索引 index 处插入一个元素
注意：允许 index 等于当前元素数量（即尾插）；index 大于 count 时返回 JC_AL__INDEX_OUT_OF_BOUNDS；时间复杂度为 O(N)
```
> int jcal_popBack(JC_ArrayList* const list)
```text
移除尾部的元素
注意：若开启了 JC_AL_FLAG_AUTO_SHRINK，会触发自动缩容；若实例已空，返回 JC_AL__ALREADY_EMPTY
```
> int jcal_popFront(JC_ArrayList* const list)
```text
移除头部的元素
注意：时间复杂度为 O(N)；若实例已空，返回 JC_AL__ALREADY_EMPTY
```
> int jcal_removeAt(JC_ArrayList* const list, size_t index)
```text
移除索引 index 处的元素
注意：index 越界返回 JC_AL__INDEX_OUT_OF_BOUNDS；若开启了 JC_AL_FLAG_AUTO_SHRINK，会触发自动缩容；时间复杂度为 O(N)
```
> const char* jcal_statusToCharArray(int status)
```text
返回状态码对应的可读字符串描述
注意：返回的是静态字符串，调用者无需也不应释放
```