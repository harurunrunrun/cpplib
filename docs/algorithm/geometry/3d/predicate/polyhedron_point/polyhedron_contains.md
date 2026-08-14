---
title: General Polyhedron Point Containment (一般多面体の点包含)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_point/polyhedron_contains.hpp
---

凸性を仮定しない閉三角形メッシュについて、点が境界または内部にあるかを一般化立体角で判定する。

## API

- `polyhedron_contains(polyhedron, point)`: 境界を含む立体内なら `true`。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間計算量 $O(V+F)$、追加空間計算量 $O(1)$。

## 注意点

面は非退化で、閉じた一貫した向きの三角形メッシュを作る必要がある。凸性は要求しない。
非有限入力・不正indexを拒否する。境界は `GEOMETRY3D_EPS` の許容誤差を含む。
