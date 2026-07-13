---
title: Longest Increasing Subsequence
documentation_of: ../src/algorithm/other/longest_increasing_subsequence.hpp
---

最長増加部分列を復元する。返り値は元の列における添字列である。

```cpp
auto indices = longest_increasing_subsequence(values);
auto nondecreasing = longest_increasing_subsequence(values, false);
int length = longest_increasing_subsequence_length(values);
```

`strict=true` なら狭義増加、`false` なら広義増加を扱う。
第3引数に順序を表す比較関数を渡せる。

## 計算量

入力長を $N$、答えの長さを $L$ とする。

| 操作 | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `longest_increasing_subsequence` | $O(N\log(L+1))$ | $O(N)$ |
| `longest_increasing_subsequence_length` | $O(N\log(L+1))$ | $O(L)$ |

比較関数1回の計算量は $O(1)$ とする。
