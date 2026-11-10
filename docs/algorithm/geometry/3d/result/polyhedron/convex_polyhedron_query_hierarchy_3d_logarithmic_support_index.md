---
title: Convex Polyhedron Logarithmic Support Index (凸多面体の対数時間支持点添字)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron/convex_polyhedron_query_hierarchy_3d_logarithmic_support_index.hpp
---

`ConvexPolyhedronQueryHierarchy3D` の内部メンバ関数の定義で、極（polar）階層を使って支持点の頂点添字を求める。

## API

- `ConvexPolyhedronQueryHierarchy3D::logarithmic_support_index(direction)`: 厳密に表した非零の方向 `direction` を極階層上で定位し、その方向の支持点となる頂点添字を返す private メンバ関数。支持点が複数ある（方向が辺や面に垂直な）場合は、最小の頂点添字を返す。

## API別の時間計算量・空間計算量

面数を $F$ とすると、最悪時間 $O(\log F)$（多倍長 dyadic 演算の回数で数える）、追加空間 $O(1)$。

## 注意点

- private メンバ関数の実装を分割したヘッダである。公開されているクエリとしては `support_point(direction)` を使う。
- 零方向や階層ラベルの破損は内部不変条件の違反とみなし、`std::logic_error` を送出する。
