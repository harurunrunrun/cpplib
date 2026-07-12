---
title: Dynamic Rectangle Add Point Get
documentation_of: ../src/algorithm/other/dynamic_rectangle_add_point_get.hpp
---

長方形加算・点取得。

更新に現れる長方形を `reserve_rectangle` で登録してから `build` する。

# コンストラクタ

```cpp
DynamicRectangleAddPointGet<T, Coordinate> seg
```

# 関数

```cpp
seg.reserve_rectangle(x1, y1, x2, y2)
seg.build()
seg.add(x1, y1, x2, y2, value)
seg.get(x, y)
```

長方形は半開領域 $[x1, x2) \times [y1, y2)$ として扱う。

## 時間計算量

- `build`: $O(N \log^2 N)$
- `add`, `get`: $O(\log^2 N)$
