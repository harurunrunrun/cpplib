---
title: Dynamic Point Add Rectangle Sum
documentation_of: ../src/algorithm/other/dynamic_point_add_rectangle_sum.hpp
---

点加算・長方形和。

更新に現れる点を `reserve_point` で登録してから `build` する。

# コンストラクタ

```cpp
DynamicPointAddRectangleSum<T, Coordinate> seg
```

# 関数

```cpp
seg.reserve_point(x, y)
seg.build()
seg.add(x, y, value)
seg.prefix_sum_less(x, y)
seg.prefix_sum_leq(x, y)
seg.sum(x1, y1, x2, y2)
```

`sum(x1, y1, x2, y2)` は半開長方形 $[x1, x2) \times [y1, y2)$ の和を返す。

## 時間計算量

- `build`: $O(N \log^2 N)$
- `add`, `sum`: $O(\log^2 N)$
