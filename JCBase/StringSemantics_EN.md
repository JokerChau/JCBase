# StringSemantics
## Purpose of This Document
This document defines the semantic rules and behavioral conventions of the String module in this library. It focuses on *why* the semantics are defined this way and how different interfaces stay consistent. For per-function return-value details, refer to `String.md`.

## General Concepts
### The Three Kinds of "Empty"
A String instance can be "empty" in three distinct ways:

1. **Null instance (`str == NULL`)**: the `JC_String*` pointer is NULL, no instance exists.
2. **Null buffer (`str->buffer == NULL`)**: the instance exists, but its internal `buffer` is NULL, content is inaccessible.
3. **Empty content (`str->buffer[0] == '\0'`)**: the instance exists, `buffer` is valid, content length is 0.

The three are handled in three different ways:
- **Identity operations** (`equals`, `contains`, `compare`): the three kinds are mutually unequal; the same kind is equal.
- **Query operations** (`isEmpty`, `getLength`): all three are treated as "empty".
- **Transformation operations** (`append`, `replace`, `trim`, `sub`, `reverse`, etc.): each follows its own rule, returning NULL, an exception-status instance, or a normal result.

### Instance Usability
An instance is **usable** if the instance pointer is non-NULL and `buffer` is non-NULL; its content is readable and operable.

**Subject usability** applies to append/replace-style interfaces whose first parameter is the subject: if the subject is unusable, the operation cannot proceed, returning NULL (null instance) or an exception-status instance (null buffer). When a non-subject parameter is unusable, the subject content is preserved, and the function returns "subject copy + parameter error status".

### JC_SpaceMode
`JC_SpaceMode` controls whether character classification follows the current locale:

- `JC_SPACE_ASCII`: uses only the fixed ASCII character set (whitespace: space, `\t`, `\n`, `\r`, `\v`, `\f`; case: `A-Z`, `a-z`).
- `JC_SPACE_LOCALE` or unknown modes: follows the current locale's `isspace`, `tolower`, `toupper`.

Affected interfaces: `trim`, `trimFront`, `trimBack`, `isBlank`, `equalsIgnoreCase*`, `containsIgnoreCase*`, `compareIgnoreCase*`, `toLowerCase`, `toUpperCase`, `toInt`, `toLong`, `toLongLong`, `toFloat`, `toDouble`.

### Success Code
All successful operations return `JC_STR__SUCCESSFUL_OP` (`split` partial success returns `JC_STR__PARTIAL_SPLIT`). Dedicated success codes such as `SUCCEED_TO_INT` are no longer used. The caller's success check is uniformly "return code == `SUCCESSFUL_OP`".

## Specific Interfaces and Semantics

### append Family
Follows **leader first, follower next**: the first parameter (leader) determines whether a result instance can exist; the second parameter (follower) determines whether the concatenation can complete.

**Subject rule (str1 / str)**
- Null instance: return NULL (no result can be constructed).
- `buffer == NULL`: return an instance with status `JC_STR__NULL_BUFFER` (subject inaccessible).

**Follower rule (str2 / content)**
- Follower is NULL or `buffer == NULL`: return "subject copy + `JC_STR__INVALID_FOLLOWER` / `JC_STR__INVALID_FOLLOWER_BUFFER`". Subject content is preserved; the error is signalled through the status code.
- Follower is an empty string (`buffer != NULL && buffer[0] == '\0'`): treated as the identity element, returns a copy of the subject with status `AVAILABLE`.

**Normal concatenation**: returns a new instance containing str1 + str2, status `AVAILABLE`.

### reverse
Three kinds of empty handled individually:
- `str == NULL`: return NULL.
- `buffer == NULL`: return `JC_STR__NULL_BUFFER` instance.
- Empty string: return an empty string instance (`AVAILABLE`).
- Otherwise: return a new instance with reversed content.

### split
Follows the **faithful-return** rule: no element failure is hidden.

- `outArray` / `count` is NULL: return `JC_STR__NULL_ARRAY` / `JC_STR__NULL_COUNT` without modifying the other output parameter.
- `str == NULL` / `buffer == NULL`: return the corresponding error code, `*outArray = NULL`, `*count = 0`.
- All segments filtered out: return `JC_STR__SUCCESSFUL_OP` with `*outArray = NULL`, `*count = 0` (success but empty result).
- Some element allocation failed: return `JC_STR__PARTIAL_SPLIT`; the corresponding position may be NULL or an instance with status `JC_STR__NULL_BUFFER`.
- All successful: return `JC_STR__SUCCESSFUL_OP`.

**Empty strings**: when `removeEmptyEntries == false`, empty segments are kept as empty string instances; when `true`, they are dropped.

**Ownership**: the caller is responsible for `jcstr_delete`-ing each element and `free`-ing the array itself.

### trim Family
Three kinds of empty handled individually:
- `str == NULL`: return NULL.
- `buffer == NULL`: return `JC_STR__NULL_BUFFER` instance.
- Empty string: return an empty string instance.
- All whitespace: return an empty string instance (content cleared).
- Otherwise: return a new instance with the corresponding side trimmed.

Whitespace classification is controlled by `mode`. `trim` / `trimFront` / `trimBack` and `isBlank` use the same character set, ensuring "`isBlank` says all-whitespace, `trim` can clear".

### charAt
Rejects unusable instances:
- `result == NULL` → `JC_STR__NULL_CHAR`.
- `str == NULL` → `JC_STR__NULL`.
- `buffer == NULL` → `JC_STR__NULL_BUFFER`.
- `index` out of bounds → `JC_STR__IDX_OUT_BOUNDS`.

### indexOf Family
Rejects instances whose content is inaccessible:
- `result == NULL` → `JC_STR__NULL_RECEIVER`.
- `str == NULL` → `JC_STR__NULL`.
- `buffer == NULL` → `JC_STR__NULL_BUFFER`.
- Not found → `JC_STR__WITHOUT_CH`, `*result` unmodified.

**Empty-substring semantics**: in `WithInst` / `WithCh` versions, an empty substring is treated as matching at the **end**, `*result = str->cachedLen`. This is an intentional semantic choice, not the `strstr(str, "")` default of "match at start". "End" is chosen to align with `replace`'s empty-target semantics (append at end).

### sub Family
Three kinds of empty handled individually:
- Null instance / `ch` is NULL: return NULL or `JC_STR__NULL_BUFFER` instance.
- `buffer == NULL`: return `JC_STR__NULL_BUFFER` instance.
- Empty string / `length == 0` / `start == end`: return an empty string instance.

Out-of-bounds handling: return an **empty-string instance** with status `INVALID_START` / `INVALID_END` / `INVALID_IDX` / `IDX_OUT_BOUNDS` (not NULL, not an invalid buffer). The content is truncated to the valid range. The caller receives an instance that is content-safe and status-informative.

### replace Family
**Subject rule**: `str == NULL` returns NULL; `buffer == NULL` returns a `JC_STR__NULL_BUFFER` instance.

**Parameter rule**:
- `target == NULL`: return str copy + `JC_STR__NULL_TARGET`.
- `replacement == NULL` (`WithCh`) or `replacement == NULL || replacement->buffer == NULL` (`WithInst`): return str copy + `JC_STR__INVALID_REPLACEMENT`.

`WithInst` only checks whether the replacement buffer is usable, not `replacement->status`. This matches the mutable version.

**Empty-target semantics**:
| Situation | Behavior |
|---|---|
| Empty content + empty target | Set the whole content to replacement |
| Empty content + non-empty target | Return original content copy (nothing to replace) |
| Non-empty content + empty target | Append replacement at the end (equivalent to `appendWithCh`) |

"Empty target = match at end" is consistent with `indexOf("")` returning `cachedLen`.

**Match rule**: `replaceFirst` replaces the first occurrence; `replaceAll` replaces all **non-overlapping** occurrences (`scan += targetLen`). If not found, return the original content copy.

**Self-replacement**: The immutable version **allows** `replacement` to point inside `str->buffer`, because a new buffer is allocated and the original buffer remains valid throughout. The mutable version **rejects** self-replacement at entry (returns `JC_MS__INVALID_REPLACEMENT`) because `realloc` would move the original buffer. This asymmetry is intentional and driven by the allocation model.

### startWith / endWith
Both parameters reject unusable instances (treating parameters as required, not nullable):
- `str == NULL` or `buffer == NULL`: return false.
- `affix == NULL`: return false (parameter missing, match does not hold).
- `affix` is an empty string: return true (the empty string is a prefix/suffix of every valid string).
- Otherwise: compare with `strncmp`.

The `WithInst` version delegates to `WithCh`: `prefix->buffer == NULL` is handled by the `!prefix` branch as false; `prefix->buffer == ""` is handled by the empty-string branch as true.

### isEmpty / isBlank
`isEmpty`: judges the three kinds of empty; `str == NULL`, `buffer == NULL`, `buffer[0] == '\0'` all return true.

`isBlank`: returns true if `isEmpty` is true; otherwise uses `mode` to check whether the content consists solely of whitespace. `isBlank` includes empty.

### equals
**Rule**:
| str1 | str2 | Result |
|---|---|---|
| NULL | NULL | true |
| NULL | non-NULL | false |
| buffer == NULL | buffer == NULL | true |
| buffer == NULL | buffer != NULL | false |
| empty string | empty string | true |
| empty string | non-empty | false |
| non-empty | non-empty | compare content |

The `WithCh` version treats `char*` as a content-layer presence: `str->buffer == NULL && ch == NULL` returns true (same kind of absence); a single-side NULL returns false. This is a different layer from `WithInst`'s instance-layer judgment, and the two are not symmetric.

The `IgnoreCase` version compares lengths first, then character-by-character using `toLowerInternal(..., mode)`.

### contains
Same rule as `equals`:
- Both NULL → true (NULL contains NULL).
- One NULL → false.
- Both `buffer == NULL` → true.
- One `buffer == NULL` → false.
- Otherwise: `strstr(str1->buffer, str2->buffer) != NULL`.

An empty needle matches every valid string (standard `strstr` behavior, no extra handling needed).

`contains` and `equals` share the same "two kinds of absence are equal" rule: `contains(a, b)` is necessarily true when `equals(a, b)` is true.

### compare
**Rule same as `equals`**, but the result is written to `int*`, and the function returns a status code:
- `result == NULL` → `JC_STR__NULL_RECEIVER`.
- Both str NULL → `*result = 0`, `JC_STR__SUCCESSFUL_OP`.
- One str NULL → `JC_STR__NULL`.
- Both `buffer == NULL` → `*result = 0`, `JC_STR__SUCCESSFUL_OP`.
- One `buffer == NULL` → `JC_STR__NULL_BUFFER`.
- Otherwise: `*result = strcmp(...)`, `JC_STR__SUCCESSFUL_OP`.

**Correspondence with `equals`**: `equals(a, b) == (compare(a, b, &r) == SUCCESSFUL_OP && r == 0)` holds for all inputs.

The `WithCh` version mirrors `equalsWithCh`: `str->buffer == NULL && ch == NULL` sets `*result = 0` and returns `SUCCESSFUL_OP`.

### toCharArray
Returns a copy; the caller is responsible for `free`. `str == NULL` or `buffer == NULL` returns NULL; an empty string returns a freeable, non-NULL empty string.

### toNum Family
**Permits loose mathematical forms, hexadecimal, `inf` / `nan`, consistent with the current String design.**

- **Integers**: decimal; leading/trailing whitespace (controlled by `mode`) and `+/-` sign allowed; hexadecimal, underscores, and embedded whitespace rejected. Overflow returns the corresponding `OVERFLOW_*`.
- **Floating-point**: built on `strtod`; accepts `.5`, `5.`, hexadecimal floats (`0x1.8p1`), `inf` / `infinity` / `nan` (case-insensitive); `errno == ERANGE` detects overflow.

Status checks precede parsing: `str == NULL` / `buffer == NULL` / empty string → `JC_STR__EMPTY`; `status != AVAILABLE` → return `str->status`; `result == NULL` → `JC_STR__NULL_*`.

### toLowerCase / toUpperCase
Three kinds of empty handled individually:
- `str == NULL`: return NULL.
- `buffer == NULL`: return `JC_STR__NULL_BUFFER` instance.
- Empty string: return an empty string instance.
- Otherwise: return a new instance with converted content, controlled by `mode`.

### toMutableString
- `str == NULL`: return NULL.
- `buffer == NULL`: return a mutable string instance with status `JC_MS__NULL_BUFFER`.
- Otherwise: return a mutable string instance with the same content.

## Invariants
The library maintains the following invariants; all interface behavior assumes them:

1. `status == JC_STR__AVAILABLE` implies `buffer != NULL`.
2. `cachedLen == strlen(buffer)` (when `buffer != NULL`).
3. `buffer` contains no embedded `'\0'`; the terminator appears only at `buffer[cachedLen]` (maintained by mechanisms such as `changeAt` rejecting `'\0'`).
4. Every operation in the immutable version that produces a new instance leaves the original instance unmodified.