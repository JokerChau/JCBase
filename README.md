\# JCBase



一个 C 语言基础工具库，提供动态字符串、泛型数组和安全的输入解析。



\*\*设计目标\*\*：在标准 C 的基础上，提供一套带状态检查的安全字符串操作、泛型容器和统一的错误处理机制，方便在多个 C 项目中复用。



\---



\## 模块说明



\### String（动态字符串）



\- 自动内存管理（创建、销毁、扩容）

\- 支持拼接、反转、分割、修剪、大小写转换

\- 内置状态码（`AVAILABLE` / `NULLBUFFER` / `EMPTYSTRING` / `BLANKSTRING`）

\- 缓存字符串长度，避免重复 `strlen`

\- 安全类型转换：`toInt` / `toLong` / `toLongLong` / `toFloat` / `toDouble`（含溢出检测）



\### ArrayList（泛型动态数组）



\- 存储任意类型数据（通过 `void\*` + `typeSize` 实现）

\- 自动扩容

\- 状态码反馈（`AVAILABLEAL` / `NULLAL` / `ALLOCALFAIL` 等）



\### Scanner（输入扫描）



\- `nextLine()`：读取一行输入，返回 `String\*`

\- （预留 `nextInt` / `nextLong` 扩展接口）



\---



\## 如何使用



\### 方式一：直接复制源码（最简单）



将 `src/` 目录下的所有 `.c` 和 `.h` 文件复制到你的项目中，与你的源码一起编译即可。



```bash

gcc -o myapp \*.c

\### 方式二：编译为静态库（推荐）

bash

\# 编译静态库

gcc -c -O2 ArrayList.c String.c Scanner.c

ar rcs libjcbase.a \*.o



\# 在你的项目中使用

gcc -o myapp myapp.c -L. -ljcbase

\### 方式三：编译为动态库（Windows DLL）

bash

gcc -shared -O2 -DWHEELS\_EXPORTS -o jcbase.dll ArrayList.c String.c Scanner.c -Wl,--out-implib,libjcbase.a

然后在你的项目中：



c

\#include "ArrayList.h"

\#include "String.h"

\#include "Scanner.h"

编译时链接 jcbase.dll（或 .lib）。



快速示例

动态字符串

c

\#include "String.h"



int main() {

&#x20;   string s = newStringFrom("Hello");

&#x20;   string t = newStringFrom(" World");

&#x20;   string combined = appendString(s, t);



&#x20;   printf("%s\\n", getStringContent(combined));



&#x20;   deleteString(s);

&#x20;   deleteString(t);

&#x20;   deleteString(combined);

&#x20;   return 0;

}

字符串转整数（带错误检测）

c

\#include "String.h"



int main() {

&#x20;   string s = newStringFrom("12345");

&#x20;   int result;

&#x20;   int status = toInt(s, \&result);



&#x20;   if (status == SUCCEEDTOINT) {

&#x20;       printf("转换成功：%d\\n", result);

&#x20;   } else {

&#x20;       printf("转换失败，状态码：%s\\n", stringStatusToArray(status));

&#x20;   }



&#x20;   deleteString(s);

&#x20;   return 0;

}

泛型数组

c

\#include "ArrayList.h"



int main() {

&#x20;   ArrayList\* list = alCreate(sizeof(int));



&#x20;   int values\[] = {10, 20, 30};

&#x20;   for (int i = 0; i < 3; i++) {

&#x20;       alPushBack(list, \&values\[i]);

&#x20;   }



&#x20;   printf("元素数量：%zu\\n", getAlCount(list));



&#x20;   alDestroy(list);

&#x20;   return 0;

}

状态码说明

所有函数通过返回值或 status 字段反馈执行结果。



状态码	含义

AVAILABLE / AVAILABLEAL	操作成功，实例可用

NULLBUFFER	字符串内容为空指针

NULLSTRING / NULLAL	实例指针为空

EMPTYSTRING	字符串内容为空串

BLANKSTRING	字符串内容为空白字符

OVERFLOWINT / OVERFLOWLONG / OVERFLOWLONGLONG	数值转换时溢出

NOTINT / NOTLONG	字符串内容不是有效数字

IDXOUTBOUNDS	索引越界

ALLOCALFAIL	内存分配失败

完整的错误码定义请参考 resources.h。



编译要求

C99 或更高版本



Windows + MinGW / MSVC（Linux 下移除 \_\_declspec 即可）



无额外依赖



目录结构

text

JCBase/

├── ArrayList.c / .h   # 泛型数组

├── String.c / .h      # 动态字符串

├── Scanner.c / .h     # 输入扫描

├── resources.h        # 统一状态码

├── wheel\_ex.h         # 导出宏控制

└── README.md

版本信息

v1.0：基础功能（String + ArrayList + Scanner）



作者

JokerChau

