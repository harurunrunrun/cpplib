---
title: Seeded 3D Convex Hull (seed指定三次元凸包)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/point_set/convex_hull_3d_with_seed.hpp
---

呼び出し側が指定したシードで増分順序をシャッフルし、乱択増分法で点集合の三次元凸包を構築する。

## API

- `convex_hull_3d_with_seed(input, random_seed)`: 完全に一致する点を 1 つにまとめ、極点と外向き三角形面を持つ `ConvexPolyhedron3` を返す。`random_seed` は `std::uint64_t` である。同じ入力点列とシードに対しては、結果の順序も再現できる。

## API 別の時間計算量・空間計算量

入力点数を $N$ とする。

- アフィン次元が 2 以下: 時間 $O(N \log N)$、追加領域 $O(N)$。
- アフィン次元が 3: シードが入力と独立な一様乱数なら期待時間 $O(N \log N)$、期待追加領域 $O(N)$。最悪時間は $O(N^2 \log N)$、最悪追加領域は $O(N^2)$。

## 注意点

- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 期待計算量が必要なら、入力と独立なシードを用いる。
- 面の向きとアフィン次元は exact dyadic 演算で判定する。
