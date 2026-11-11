# StringSemantics
## 此文本作用
> 阐述该库String类模块对某些接口的语义定义，或是某些函数行为的规范
## 具体接口与语义或规范
### append类
> 该类接口遵循 leader first、follower next，关注实例是否可用，且实例可用才可操作<br>
leader first、follower next 即首参数会优先考虑，返回的实例是否为NULL取决于首参数<br>
实例可用才可操作即当实例能访问且其buffer非NULL，则表示该实例可被append族接口使用
### reverse
> 该接口在接受到 NULL 或 buffer 为 NULL 的实例时按照语义返回 NULL 或 buffer 为 NULL 的异常实例
### split
> 该接口遵循如实返回的规范，当部分分配内存失败时，该元素对应索引当为NULL
### trim
> 该接口遵循语义为三种空（NULL/buffer == NULL/buffer[0] == '\0'）各自返回对应实例或NULL
### indexOf
> 该接口不接受内容不可操作的实例
### sub
> 该接口遵循语义为三种空（NULL/buffer == NULL/buffer[0] == '\0'）各自返回对应实例或NULL
### replace
> 当实例不可用时，返回相关实例或 NULL，当 target 为空内容也为空时，整体设置 replacement，当 target 为空内容不为空时末尾追加
### startWith/endWith
> 两个参数都完全拒绝不可操作实例（NULL 或 buffer 为 NULL 的实例）
### isEmpty
> 判断库中定义的三种空，NULL/buffer == NULL/buffer[0] == '\0'
### isBlank
> 通过 mode 选择是按默认的 locale 还是按照 ASCII 或是其他编码认可的执行
### equals
> NULL == NULL、nullBuffer == nullBuffer、空内容 == 空内容
### contains
> 判定同equals
### compare
> 返回状态码，结果用指针返回，判定同equals
### toCharArray
> 返回拷贝
### toNum
> 允许非严谨数学式，允许16进制，inf和nan与String现有设计一致