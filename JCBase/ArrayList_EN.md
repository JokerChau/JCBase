# ArrayList
## Other Languages md
### 简体中文
[ArrayList.md 简中版](ArrayList.md)
## Interface Description
> jc_ArrayList jcal_inner_new(size_t sizeType)
```text
This is an internal function. It returns a stateful jc_ArrayList instance with empty data. The size of the type must be passed in.
Note: If other parameters or the void type are passed in, the library is not responsible for any unexpected consequences!!!
```
> jc_ArrayList jcal_newOf(type)
```text
This is a public library interface. It returns a stateful jc_ArrayList instance with empty data. A type or variable must be passed in.
Note: Try not to pass in void; if a pointer type is passed in, an "array of pointers" instance will be created.
```
> void jcal_delete(jc_ArrayList list)
```text
This is a public library interface with no return value. A jc_ArrayList instance must be passed in.
Note: Passing in NULL will not crash, and passing in an instance whose data is NULL will not crash, but do not pass in a non-jc_ArrayList variable that has been cast to "jc_ArrayList".
```
> size_t jcal_getLength(const jc_ArrayList list)
```text
This is a public library interface. It returns the number of elements in the dynamic array instance as size_t. A jc_ArrayList instance must be passed in.
Note: When list is NULL, it returns 0; when list->data is NULL, it also returns 0.
```
> size_t jcal_getMaxLength(const jc_ArrayList list)
```text
This is a public library interface. It returns the maximum capacity of the dynamic array instance as size_t. A jc_ArrayList instance must be passed in.
Note: When list is NULL, it returns 0.
```
> int jcal_getStatus(const jc_ArrayList list)
```text
This is a public library interface. It returns a status code of type int. A jc_ArrayList instance must be passed in.
Note: The library's status codes are represented by an enum, so the return value of this interface can be received directly with "int status", or with JC_ArrayListStatus.
```
> int jcal_setMaxLength(jc_ArrayList list, size_t count)
```text
This is a public library interface. It returns a status code of type int. A jc_ArrayList instance must be passed in.
Note: The count passed in must not be less than the current number of elements in the instance. If the instance status is abnormal, the operation will not be executed. When the preceding condition is satisfied, if count is 0, it is regarded as clearing data.
```
> int jcal_clear(jc_ArrayList list)
```text
This is a public library interface. It returns a status code of type int. A jc_ArrayList instance must be passed in.
Note: This operation does not free memory. If redundant memory needs to be freed, please use jcal_shrinkToFit.
```
> int jcal_shrinkToFit(const jc_ArrayList list)
```text
This is a public library interface. It returns a status code of type int. A jc_ArrayList instance must be passed in.
Note: This operation will free redundant memory. If the instance status is abnormal, this operation will not be executed.
```