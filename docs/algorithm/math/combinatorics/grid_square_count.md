---
title: Grid Square Count (正方格子に含まれる正方形数) [SAMER08F]
documentation_of: ../../../../src/algorithm/math/combinatorics/grid_square_count.hpp
---

正方格子に含まれる軸平行な正方形の総数を返す。

## grid_square_count

```cpp
long long grid_square_count(long long side_length);
```

一辺が単位正方形 `side_length` 個分の正方格子について、含まれる正方形の総数 $\sum_{i=1}^{n} i^2 = n(n+1)(2n+1)/6$（$n$ は `side_length`）を返す。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `grid_square_count` | $O(1)$ | $O(1)$ |

## 注意点

- 辺長が負なら `runtime_error`、結果が `long long` に収まらなければ `overflow_error` を送出する。
