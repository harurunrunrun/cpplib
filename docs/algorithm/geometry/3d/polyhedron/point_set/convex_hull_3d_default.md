---
title: Deterministic 3D Convex Hull (決定的三次元凸包)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/point_set/convex_hull_3d_default.hpp
---

決定的なkinetic divide-and-conquer法により、点集合の三次元凸包を構築する。

## API

- `convex_hull_3d(points)`: 完全一致する点を重複除去し、極点と外向き三角形面を持つ `ConvexPolyhedron3` を返す。空集合の `affine_dimension` は $-1$、一点・共線・共面・三次元ではそれぞれ $0,1,2,3$。

## API別の時間計算量・空間計算量

入力点数を $N$ とする。

- 全アフィン次元: 最悪時間 $O(N\log N)$、追加領域 $O(N)$。
- 三次元では二つのlower hullを線形mergeする分割統治と、facetの正規化を行う。

## 注意点

座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
面の向きとアフィン次元はexact dyadic predicateで判定する。同じ点集合からは
入力順によらず同じ頂点列・面列を返す。exact整数演算のビット計算量は上記の
RAM計算量に含めない。
