---
title: Seeded 3D Convex Hull (seed指定三次元凸包)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/point_set/convex_hull_3d_with_seed.hpp
---

呼出側が指定したseedで増分順序をshuffleし、点集合の三次元凸包を構築する。

## API

- `convex_hull_3d_with_seed(points, random_seed)`: 完全一致する点を重複除去し、極点と外向き三角形面を持つ `ConvexPolyhedron3` を返す。同じ入力点列とseedに対する結果順序は再現可能である。

## API別の時間計算量・空間計算量

入力点数を $N$ とする。

- アフィン次元2以下: 時間 $O(N\log N)$、追加領域 $O(N)$。
- アフィン次元3: seedが入力から独立な一様乱数なら期待時間 $O(N\log N)$、期待追加領域 $O(N)$。最悪時間 $O(N^2\log N)$、最悪追加領域 $O(N^2)$。

## 注意点

座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。期待計算量が必要なら入力から独立なseedを用いる。面の向きとアフィン次元はexact dyadic predicateで判定する。
