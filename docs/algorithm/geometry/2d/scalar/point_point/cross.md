---
title: 2D Cross Product (二次元外積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_point/cross.hpp
---

2 次元ベクトルの外積を求める。

## API

```cpp
long double cross(const Point& left, const Point& right);
```

符号付きのスカラー値 $\mathit{left}_x \cdot \mathit{right}_y - \mathit{left}_y \cdot \mathit{right}_x$ を返す。

## 時間計算量

時間計算量 $O(1)$、追加領域 $O(1)$。

## 注意点

正の値は `right` が `left` から見て反時計回り側にあることを表す。
