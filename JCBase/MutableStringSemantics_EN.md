# MutableStringSemantics
## Purpose of This Document
This document defines the semantic rules and behavioral conventions of the MutableString module in this library. It focuses on *why* the semantics are defined this way and how different interfaces stay consistent. For per-function return-value details, refer to `MutableString.md`. Many rules are shared with `StringSemantics.md`; the two documents should be read together.

## General Concepts
### The Three Kinds of "Empty"
A MutableString instance can be "empty" in three distinct ways:

1. **Null instance (`ms == NULL`)**: the `JC_MutableString*` pointer is NULL, no instance exists.
2. **Null buffer (`ms->buffer == NULL`)**: the instance exists, but its internal `buffer` is NULL, content is inaccessible.
3. **Empty content (`ms->len == 0`, equivalently `ms->buffer[0] == '\0'`)**: the instance exists, `buffer` is valid, `len` is 0.

The three are handled in three ways, consistent with the String module:
- **Identity operations** (`equals`, `contains`, `compare`): mutually unequal; the same kind is equal.
- **Query operations** (`isEmpty`, `getLength`): all three treated as "empty".
- **Transformation operations** (`append`, `replace`, `trim`, `sub`, `reverse`, etc.): each follows its own rule.

### Instance Usability
An instance is **usable** if the instance pointer is non-NULL and `buffer` is non-NULL.

### In-Place Modification vs New Instance
MutableString interfaces fall into two categories:

- **In-place modification**: `setContent`, `changeAt`, `append*`, `reverse`, `trim*`, `replace*`, `toLowerCase`, `toUpperCase`. On success, `ms` itself is modified. On failure, `ms` is guaranteed to retain its pre-operation state (no partial results).
- **New-instance return**: `clone`, `sub*`, each element of `split`, the result of `toString`, the result of `toCharArray`. `ms` is not modified; an independent instance is returned.

### JC_SpaceMode
Same as the String module. Affected interfaces: `trim`, `trimFront`, `trimBack`, `isBlank`, `equalsIgnoreCase*`, `containsIgnoreCase*`, `compareIgnoreCase*`, `toLowerCase`, `toUpperCase`, `toInt`, `toLong`, `toLongLong`, `toFloat`, `toDouble`.

### Success Code
All successful operations return `JC_MS__SUCCESSFUL_OP` (`split` partial success returns `JC_MS__PARTIAL_SPLIT`). Legacy success codes such as `MS_SUCCEEDTOINT` are deprecated; the caller's success check is uniformly "return code == `SUCCESSFUL_OP`".

### status Field Check Policy
The mutable version handles `status != AVAILABLE` in two ways:

- **Write operations that depend on instance integrity**: `setContent`, `changeAt`, `replace*` check `ms->status != AVAILABLE` at entry and return `ms->status` when abnormal. Rationale: writing to an instance marked untrustworthy would break invariants.
- **Operations computed from content**: `reverse`, `trim*`, `toLowerCase`, `toUpperCase` do not check `status`, and only fail when `buffer == NULL`. Rationale: these read content itself; as long as `buffer` is readable, conversion breaks nothing. An abnormal `status` signals "the previous operation failed", not "the content is unusable".

The immutable version adopts the same split (`toLowerCase` etc. do not check `status`; `toInt` etc. do).

## Specific Interfaces and Semantics

### Construction and Capacity
`jcms_new`: returns an empty instance with capacity `INITIAL_MS_SIZE + 1`.

`jcms_newFrom`: capacity `max(strlen(content), INITIAL_MS_SIZE) + 1`. This is the only construction path that reserves a large capacity in one shot; subsequent appends do not trigger `realloc` while capacity suffices.

Capacity growth strategy (inside `appendCore` / `setContent`):
- `setContent`: `realloc` to `ceilAlignInitSize(needed)` only when capacity is insufficient, aligned to a multiple of `INITIAL_MS_SIZE`, without extra growth slack.
- `appendCore`: when `srcLen` is large, aligns precisely to `needed`; when `srcLen` is small, grows by 1.25x; otherwise grows by 2x. All three align to `INITIAL_MS_SIZE`.

### append Family
**Subject rule (ms)**
- `ms == NULL`: return `JC_MS__NULL`.
- `ms->buffer == NULL`: return `JC_MS__NULL_BUFFER`.

**Follower rule**
- `src == NULL` (`WithInst`): return `JC_MS__NULL_FOLLOWER`.
- `src->buffer == NULL` (`WithInst`): return `JC_MS__NULL_FOLLOWER_BUFFER`.
- `ch == NULL` (`WithCh`): return `JC_MS__NULL_FOLLOWER`.
- `src->len == 0` or `ch[0] == '\0'`: return `SUCCESSFUL_OP`, no modification (empty string is the identity element).

**Self-append**:
- `appendWithInst(ms, ms)` is **allowed**. `appendCore` accepts `char**`; when `src == ms`, `&src->buffer` and `&ms->buffer` are the same address, and after `realloc` `*src` is automatically updated to the new address, so it does not dangle.
- `appendWithCh(ms, ms->buffer + offset)` is **rejected**, returning `JC_MS__INVALID_SOURCE`. A `const char*` parameter cannot be automatically updated after `realloc`; to avoid dangling, the entry directly rejects `ch` pointing inside `ms->buffer`.

This asymmetry is **intentional**: `WithInst`'s double pointer happens to handle the alias; `WithCh`'s `const char*` cannot.

**Differences from the immutable version**: The immutable `append` returns a new instance; the mutable one modifies `ms` in place and returns a status code. On failure, the mutable `ms` retains its original state; the immutable version returns a new instance carrying the error status. Semantically corresponding, formally different.

### setContent
Sets the content of `ms` to a copy of `content`.

**content is NULL**: equivalent to "clear" - `buffer[0] = '\0'`, `len = 0`, but **does not free `buffer`, does not modify `cap`**. The caller can repeatedly call `setContent(ms, NULL)` to clear without losing capacity.

**status check**: checks `ms->status != AVAILABLE` at entry; on abnormal, returns `ms->status`.

**When capacity is insufficient**: `realloc` to `ceilAlignInitSize(needed)`; on failure returns `JC_MS__REALLOCATION_FAILED`, `ms` unchanged.

### changeAt
**status check**: checks `ms->status != AVAILABLE` at entry; on abnormal, returns `ms->status`.

**Rejects `\0`**: `ch == '\0'` returns `JC_MS__INVALID_CHARACTER_FOR_CHANGE`. This enforces the invariant "no embedded `'\0'` in content", which in turn maintains `len == strlen(buffer)`.

### reverse
**Does not check status**, fails only when `buffer == NULL`.

Three kinds of empty handled individually:
- `ms == NULL`: return `JC_MS__SUCCESSFUL_OP` (nothing to do).
- `buffer == NULL`: return `JC_MS__SUCCESSFUL_OP` (nothing to do).
- Empty string: return `JC_MS__SUCCESSFUL_OP` (nothing to do).

This differs from the immutable version: `reverse(NULL)` there returns NULL, `reverse(bufferNull)` returns a `NULL_BUFFER` instance. The mutable version operates in place; "nothing to change" is directly treated as success.

### split
Behaves identically to the immutable `split`:
- `outArray` / `count` is NULL: return `JC_MS__NULL_ARRAY` / `JC_MS__NULL_COUNT`.
- `ms == NULL` / `buffer == NULL`: return the corresponding error code, output parameters set to `NULL` / 0.
- All segments filtered out: return `SUCCESSFUL_OP` with `*outArray = NULL`, `*count = 0`.
- Some element construction failed: return `JC_MS__PARTIAL_SPLIT`; the corresponding position may be NULL or an abnormal-status instance.
- All successful: return `SUCCESSFUL_OP`.

**Ownership**: the caller is responsible for `jcms_delete`-ing each element and `free`-ing the array itself.

### trim Family
**Does not check status**, returns `SUCCESSFUL_OP` directly when `buffer == NULL` or `len == 0` (nothing to do).

In-place modification: after trimming the corresponding side, `memmove` shrinks the content, `buffer[newLen] = '\0'`, update `len`. When entirely whitespace, `buffer[0] = '\0'`, `len = 0`, but **does not free buffer, does not modify cap**.

Whitespace classification is controlled by `mode`, sharing the same source as `isBlank`.

### charAt
Same as the immutable version: `result == NULL` → `JC_MS__NULL_CHAR`; `ms == NULL` → `JC_MS__NULL`; `buffer == NULL` → `JC_MS__NULL_BUFFER`; `index` out of bounds → `JC_MS__INDEX_OUT_OF_BOUNDS`.

### indexOf Family
Same as the immutable version. Empty substrings in `WithInst` / `WithCh` are treated as matching at the **end**, `*result = ms->len`.

`result == NULL` in the mutable version returns `JC_MS__NULL_RESULT` (the immutable version uses `JC_STR__NULL_RECEIVER`). The naming mismatch is historical; the semantics are identical: "the receiving output pointer is NULL".

### sub Family
**Returns a new instance, does not modify the original `ms`**. Same semantics as the immutable `sub`:
- `ms == NULL`: return NULL.
- `buffer == NULL`: return `JC_MS__NULL_BUFFER` instance.
- Empty string / `length == 0` / `start == end`: return an empty string instance.
- Out of bounds: return an empty-string instance with status `INVALID_START` / `INVALID_END` / `INVALID_INDEX` / `INDEX_OUT_OF_BOUNDS`, content truncated to the valid range.

### replace Family
**status check**: checks `ms->status != AVAILABLE` at entry; on abnormal, returns `ms->status`.

**Parameter rule**:
- `target == NULL` → `JC_MS__NULL_TARGET`.
- `replacement == NULL` (`WithCh`) or `replacement == NULL || replacement->buffer == NULL` (`WithInst`) → `JC_MS__INVALID_REPLACEMENT`. `WithInst` only checks whether the buffer is usable, not `replacement->status`, consistent with the immutable version.

**Self-replacement**: **rejected**. When `replacement` points inside `ms->buffer`, `JC_MS__INVALID_REPLACEMENT` is returned. Rationale: the non-equal-length path needs `realloc`, and after `realloc` moves the block, `replacement` dangles. This is intentionally asymmetric with the immutable version's "allowed" - the immutable version allocates a new buffer, so the original buffer stays valid throughout.

**Empty-target semantics**:
| Situation | Behavior |
|---|---|
| Empty content + empty target | Set the whole content to replacement |
| Empty content + non-empty target | `NO_TARGET_MATCHED`, `ms` unchanged |
| Non-empty content + empty target | Append replacement at the end (equivalent to `appendCore`) |

**Non-empty target**:
- `strstr` finds the first occurrence (`First`), or `memcmp` scans all non-overlapping occurrences (`All`).
- Not found → `JC_MS__NO_TARGET_MATCHED`, `ms` unchanged.
- Found → replace in place, `realloc` as needed. On failure returns `JC_MS__REALLOCATION_FAILED`, `ms` unchanged (`realloc` failure keeps the original block).

**Invariant**: every failure path guarantees that `buffer`, `len`, `cap`, `status` of `ms` are identical before and after the call.

### startWith / endWith
Same as the immutable version:
- `ms == NULL` or `buffer == NULL`: return false.
- `affix == NULL`: return false.
- `affix` is an empty string: return true.
- Otherwise compare with `strncmp`.

The `WithInst` version delegates to `WithCh`; `prefix->buffer == NULL` is handled by `WithCh`'s `!prefix` branch as false, and `prefix->buffer == ""` is handled by the empty-string branch as true.

### isEmpty / isBlank
`isEmpty`: `ms == NULL`, `buffer == NULL`, `buffer[0] == '\0'` all return true.

`isBlank`: returns true if `isEmpty` is true; otherwise uses `mode` to check whether the content consists solely of whitespace. `isBlank` includes empty.

### equals / contains / compare
All three follow the same rules as the immutable version:

- Both NULL → true / `*result = 0`.
- One NULL → false / `JC_MS__NULL`.
- Both `buffer == NULL` → true / `*result = 0`.
- One `buffer == NULL` → false / `JC_MS__NULL_BUFFER`.
- Both empty strings → true.
- One empty string → false.
- Otherwise compare content / `strcmp`.

**Correspondence**: `equals(a, b) == (compare(a, b, &r) == SUCCESSFUL_OP && r == 0)` holds for all inputs.

**Empty needle**: in `contains`, an empty needle matches every valid string (standard `strstr` behavior).

### toCharArray
Returns a copy; the caller is responsible for `free`. `ms == NULL` or `buffer == NULL` returns NULL; an empty string returns a freeable, non-NULL empty string.

### toNum Family
Aligned with the immutable version **line by line**:

- **status check**: `ms->status != AVAILABLE` returns `ms->status` (the immutable version also checks).
- **Empty check**: `ms == NULL || buffer == NULL || buffer[0] == '\0'` → `JC_MS__EMPTY`.
- **result check**: `result == NULL` → `JC_MS__NULL_*`.
- **Integers**: decimal; `+/-` sign and whitespace (controlled by `mode`) allowed; hexadecimal, underscores, and embedded whitespace rejected. Overflow returns `OVERFLOW_*`.
- **Floating-point**: built on `strtod`; accepts `.5`, `5.`, hexadecimal floats, `inf` / `nan`. `errno == ERANGE` detects overflow.

**Internal difference from the immutable version**: the mutable `parseToLL` / `parseToDouble` use `ms->len` as the boundary (no re-`strlen`); the immutable version uses `str->cachedLen`. Both rely on the "`len == strlen(buffer)`" invariant and behave equivalently.

### toLowerCase / toUpperCase
**Does not check status**, fails only when `buffer == NULL`. Rationale: computed from content; an abnormal `status` does not prevent content conversion.

In-place modification: replace `buffer[i]` character-by-character with `toLowerInternal(..., mode)` / `toUpperInternal(..., mode)`. On an empty string, the loop runs zero times and success is returned.

### toString
- `ms == NULL`: return `jcstr_newFrom(NULL)`, i.e. an immutable instance with status `JC_STR__NULL_BUFFER`.
- `buffer == NULL`: same as above.
- Otherwise: return an immutable instance with the same content, status `AVAILABLE`.

## Invariants
The library maintains the following invariants; all interface behavior assumes them:

1. `status == JC_MS__AVAILABLE` implies `buffer != NULL`.
2. `len == strlen(buffer)` (when `buffer != NULL`).
3. `cap >= len + 1` (when `buffer != NULL`), ensuring `buffer[len] == '\0'` has a slot.
4. `buffer` contains no embedded `'\0'`; the terminator appears only at `buffer[len]` (maintained by `changeAt` rejecting `'\0'`, `setContent` computing length via `strlen`, etc.).
5. In-place modification interfaces, on failure, leave `ms`'s `buffer`, `len`, `cap`, `status` identical to before the call.