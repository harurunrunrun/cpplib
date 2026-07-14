---
title: Square-free Integer Count (平方因子を持たない整数の個数) [SQFREE]
documentation_of: ../src/algorithm/math/squarefree_integer_count.hpp
---

正の square-free integer が上限以下にいくつあるかを数える。

# `squarefree_integer_count`

```cpp
uint64_t squarefree_integer_count(uint64_t n);
```

`1 <= x <= n` かつ、1 より大きい平方数で割り切れない `x` の個数を返す。`n = 0` では `0` を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `squarefree_integer_count` | $O(n^{2/5}\log n)$ | $O(n^{2/5})$ |

## 注意点

- `n <= 10^18` を前提とする。範囲外では `invalid_argument` を送出する。
