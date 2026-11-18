# 其他语言
## English
[HTUcrazyString_EN.md English version](HTUcrazyString_EN.md)

**使用JCBase的情况**
> 请添加JCBASE_EXPORTS，然后 `#include "JC_crazyString.h"`，即可使用其中的内容

**使用JCBase_crazyString的情况**
> 请添加JCBASE_crazySTRING_EXPORTS，然后 `#include "JC_crazyString.h"`，即可使用其中的内容

# 使用示例
```C
#include "JC_crazyString.h"
#include <stdio.h>

int main(void) {
	jc_czyString string = jccstr_newFrom("Hello World!", 12); // 创建一个内容为“Hello World！”的实例
	printf("%s", jccstr_viewBuffer(string)); // 利用printf进行控制台输出
	jccstr_delete(string); // 销毁实例
	return 0;
}
```