---
title: Convex Polyhedron Logarithmic Support Index (凸多面体の対数時間支持点添字)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron/convex_polyhedron_query_hierarchy_3d_logarithmic_support_index.hpp
---

polar階層から支持点添字を求める内部member定義。

## API

- `ConvexPolyhedronQueryHierarchy3D::logarithmic_support_index(direction)`: exactな非零方向をpolar階層で定位し、tie時は最小頂点添字を返すprivate member。

## API別の時間計算量・空間計算量

面数を $F$ として最悪時間 $O(\log F)$、追加領域 $O(1)$。

## 注意点

private memberの実装leafである。公開queryには `support_point(direction)` を使う。零方向や壊れた階層labelは内部不変条件違反として例外になる。
