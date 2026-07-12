---
title: Rectangle Union
documentation_of: ../src/algorithm/other/rectangle_union.hpp
---

軸平行長方形の和集合の面積。

# 関数

```cpp
rectangle_union_area<T>(rectangles)
```

`rectangles` は `RectangleUnionRectangle<T>{x1, y1, x2, y2}` の列。

長方形は半開領域 $[x1, x2) \times [y1, y2)$ として扱う。

## 時間計算量

- $O(N \log N)$
