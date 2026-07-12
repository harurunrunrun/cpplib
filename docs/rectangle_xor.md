---
title: Rectangle Xor
documentation_of: ../src/algorithm/other/rectangle_xor.hpp
---

軸平行長方形の集合の xor の面積。

奇数個の長方形に含まれる領域の面積を返す。

# 関数

```cpp
rectangle_xor_area<T>(rectangles)
```

`rectangles` は `RectangleXorRectangle<T>{x1, y1, x2, y2}` の列。

長方形は半開領域 $[x1, x2) \times [y1, y2)$ として扱う。

## 時間計算量

- $O(N \log N)$
