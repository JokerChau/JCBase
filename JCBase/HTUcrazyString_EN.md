# Other Languages
## 简体中文
[HTUcrazyString.md 简中版](HTUcrazyString.md)

**When using JCBase**
> Add JCBASE_EXPORTS, then `#include "JC_crazyString.h"` to use its contents.

**When using JCBase_crazyString**
> Add JCBASE_crazySTRING_EXPORTS, then `#include "JC_crazyString.h"` to use its contents.

# Usage Example
```C
#include "JC_crazyString.h"
#include <stdio.h>

int main(void) {
	jc_czyString string = jccstr_newFrom("Hello World!", 12); // create an instance containing "Hello World!"
	printf("%s", jccstr_viewBuffer(string)); // print to the console via printf
	jccstr_delete(string); // destroy the instance
	return 0;
}
```
