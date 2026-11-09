---
title: General Polyhedron Support Vertex (一般多面体の支持頂点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/index/polyhedron_point/polyhedron_support_vertex.hpp
---

凸性や面の接続を仮定せず、指定方向との内積が最大となる頂点の添字を返す。

## API

- `polyhedron_support_vertex(polyhedron, direction)`: 支持頂点の添字を返す。内積が等しい頂点が複数あれば、最小の添字を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$ として、時間 $O(V)$、追加領域 $O(1)$（正確な 2 進有理数の桁の領域を除く）。

## 注意点

頂点が空の場合、方向が零ベクトルの場合、方向または頂点に非有限な値がある場合は `std::invalid_argument` を送出する。比較は正確な 2 進有理数（dyadic）の内積で行うので、丸めによって最大の頂点を取り違えることはない。
