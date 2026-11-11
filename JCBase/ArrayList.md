# ArrayList
## 其他语言md
### English
[ArrayList.md English version](ArrayList_EN.md)
## 接口介绍
> jc_ArrayList jcal_inner_new(size_t sizeType)
```text
此为内部函数，返回一个带状态的空 data 的 jc_ArrayList 实例，需传入类型的大小
注意：若传入其他参数或是 void 类型，发生任何意外非该库责任！！！
```
> jc_ArrayList jcal_newOf(type)
```text
此为库对外接口，返回一个带状态的空 data 的 jc_ArrayList 实例，需要传入类型或是变量
注意：尽量不要传入void；若是传入指针类型，则会创建一个 “指针数组” 实例
```
> void jcal_delete(jc_ArrayList list)
```text
此为库对外接口，无返回值，需要传入一个 jc_ArrayList 实例
注意：传入 NULL 不会崩溃，传入 data 为 NULL 的实例不会崩溃，但请不要传入强转为 “jc_ArrayList” 的非 jc_ArrayList 变量
```
> size_t jcal_getLength(const jc_ArrayList list)
```text
此为库对外接口，返回 size_t 类型的动态数组实例的元素个数，需要传入一个 jc_ArrayList 实例
注意：当 list 为 NULL 时会返回0，当 list->data 为 NULL 时也会返回0
```
> size_t jcal_getMaxLength(const jc_ArrayList list)
```text
此为库对外接口，返回 size_t 类型的动态数组实例的最大容量，需要传入一个 jc_ArrayList 实例
注意：当 list 为 NULL 时会返回0
```
> int jcal_getStatus(const jc_ArrayList list)
```text
此为库对外接口，返回 int 类型的状态码，需要传入一个 jc_ArrayList 实例
注意：该库状态码由 enum 表示，所以可以直接用 “int status” 来接受该接口的返回值，也可以用 JC_ArrayListStatus 来接受返回值
```
> int jcal_setMaxLength(jc_ArrayList list, size_t count)
```text
此为库对外接口，返回 int 类型的状态码，需要传入一个 jc_ArrayList 实例
注意：传入的 count 不要小于实例当前元素数量，若实例状态异常则该操作将不执行，当此前条件满足时，若 count 为0，则视为清除 data
```
> int jcal_clear(jc_ArrayList list)
```text
此为库对外接口，返回 int 类型的状态码，需要传入一个 jc_ArrayList 实例
注意：此操作不会清理内存，若实例状态异常则不执行，若需要清理冗余内存，请使用 jcal_shrinkToFit
```
> int jcal_shrinkToFit(const jc_ArrayList list)
```text
此为库对外接口，返回 int 类型的状态码，需要传入一个 jc_ArrayList 实例
注意：此操作会清理冗余内存，若实例状态异常则该操作将不执行
```