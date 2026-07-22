---
title: Grid Square Count (正方格子に含まれる正方形数) [SAMER08F]
documentation_of: ../src/algorithm/math/combinatorics/grid_square_count.hpp
---

正方格子に含まれる軸平行な正方形の総数を返す。

## grid_square_count

```cpp
long long grid_square_count(long long side_length);
```

一辺が単位正方形 `side_length` 個の格子について返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `grid_square_count` | $O(1)$ | $O(1)$ |

## 注意点

- 負の辺長は `runtime_error`、結果の範囲違反は `overflow_error` となる。
