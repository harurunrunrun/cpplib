---
title: Prefix Sum
documentation_of: ../src/structure/other/prefix_sum.hpp
---

静的な一次元列・二次元行列の区間和を返す累積和。
区間は半開区間で指定する。値型 `T` はデフォルト構築、加算、減算を使える必要がある。

## PrefixSum

```cpp
PrefixSum<T, MAX_SIZE> sum(values);
sum.prefix_sum(r);
sum.sum(l, r);
sum.get(i);
```

| 操作 | 時間計算量 |
| --- | --- |
| デフォルト構築、`PrefixSum(n)` | $O(MAX\_SIZE)$ |
| `PrefixSum(values)` | $O(MAX\_SIZE+n)$ |
| `build(values)` | $O(n)$ |
| `size()`, `empty()` | $O(1)$ |
| `prefix_sum(r)` | $O(1)$ |
| `sum(l, r)` | $O(1)$ |
| `get(i)` | $O(1)$ |

保存領域は $O(MAX\_SIZE)$。

## PrefixSum2D

```cpp
PrefixSum2D<T, MAX_HEIGHT, MAX_WIDTH> sum(matrix);
sum.sum(row_begin, col_begin, row_end, col_end);
sum.get(row, col);
```

`sum` は `[row_begin, row_end) x [col_begin, col_end)` の和を返す。
$C=(MAX\_HEIGHT+1)(MAX\_WIDTH+1)$、入力の大きさを $H\times W$ とする。

| 操作 | 時間計算量 |
| --- | --- |
| デフォルト構築、`PrefixSum2D(height, width)` | $O(C)$ |
| `PrefixSum2D(matrix)` | $O(C+HW)$ |
| `build(matrix)` | $O(HW)$ |
| `height()`, `width()`, `empty()` | $O(1)$ |
| `sum(row_begin, col_begin, row_end, col_end)` | $O(1)$ |
| `get(row, col)` | $O(1)$ |

保存領域は $O(C)$。
