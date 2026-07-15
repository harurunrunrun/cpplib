---
title: Incremental Convex Hull 3D (三次元増分凸包)
documentation_of: ../src/algorithm/geometry/3d/convex_hull_3d.hpp
---

## API

- `convex_hull_3d(points)`: 重複を除去し、点集合の凸包を返す。全点一致・共線・共面も`affine_dimension`で区別する。

## API別の時間計算量・空間計算量

- 点数を$N$、生成途中を含む面数を$F$として時間$O(NF)$、領域$O(F+N)$。最悪$O(N^2)$。

## 注意点

非有限座標には`std::invalid_argument`を送出する。3次元の面は外向きで、同一支持平面上の多角形面は複数の三角形に分割される。
