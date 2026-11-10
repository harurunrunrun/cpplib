---
title: Deterministic 3D Convex Hull (決定的三次元凸包)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/point_set/convex_hull_3d_default.hpp
---

決定的な kinetic 分割統治法により、点集合の三次元凸包を構築する。実装は `divide_and_conquer_convex_hull_3d` に委譲する。

## API

- `convex_hull_3d(input)`: 完全に一致する点を 1 つにまとめ、極点と外向き三角形面を持つ `ConvexPolyhedron3` を返す。`affine_dimension` は空集合なら $-1$、1 点・共線・共面・三次元ではそれぞれ $0, 1, 2, 3$ となる。

## API 別の時間計算量・空間計算量

入力点数を $N$ とする。

- 全アフィン次元: 最悪時間 $O(N \log N)$、追加領域 $O(N)$。
- 三次元では 2 つの lower hull を線形時間でマージする分割統治と、facet の正規化を行う。

exact な整数演算のビット計算量は上記の RAM 計算量に含めない。

## 注意点

- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 面の向きとアフィン次元は exact dyadic 演算で判定する。
- 同じ点集合からは、入力順によらず同じ頂点列・面列を返す。
