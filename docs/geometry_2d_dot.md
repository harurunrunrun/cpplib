---
title: 2D Dot Product (二次元内積)
documentation_of: ../src/algorithm/geometry/2d/scalar/dot.hpp
---

## API

- `dot(left, right)`: 2ベクトルの内積を返す。

## API別の時間計算量・空間計算量

- `dot`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

結果は `long double` であり、座標積が表現可能でなければならない。
