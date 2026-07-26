---
title: 3D Convex Hull with Default Seed (固定seedによる三次元凸包)
documentation_of: ../../../../src/algorithm/geometry/3d/convex_hull_3d_default.hpp
---

固定seedの増分順序を使い、点集合の三次元凸包を再現可能に構築する。

## API

- `convex_hull_3d(points)`: 完全一致する点を重複除去し、極点と外向き三角形面を持つ `ConvexPolyhedron3` を返す。空集合の `affine_dimension` は $-1$、一点・共線・共面・三次元ではそれぞれ $0,1,2,3$。

## API別の時間計算量・空間計算量

入力点数を $N$ とする。

- アフィン次元2以下: 時間 $O(N\log N)$、追加領域 $O(N)$。
- アフィン次元3: 固定seedに対する最悪時間 $O(N^2\log N)$、最悪追加領域 $O(N^2)$。既定seedに対して敵対的でない入力では期待的に $O(N\log N)$ 時間、$O(N)$ 領域となる。

## 注意点

座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。面の向きとアフィン次元はexact dyadic predicateで判定する。同じ入力からは同じ頂点列・面列を返す。
