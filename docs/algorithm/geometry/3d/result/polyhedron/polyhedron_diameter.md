---
title: General Polyhedron Diameter (一般多面体の直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron/polyhedron_diameter.hpp
---

凸性を仮定せず、頂点集合の最遠点対をAABB branch-and-boundで求める。多面体領域の直径は頂点対で達成される。

## API

- `polyhedron_diameter(polyhedron)`: 2頂点のindexと距離を返す。頂点が2個未満なら `std::nullopt`。

## API別の時間計算量・空間計算量

- 頂点数を $V$ として構築の時間計算量 $O(V\log V)$。探索は通常枝刈りされ、最悪時間計算量 $O(V^2)$。追加空間計算量 $O(V)$。

## 注意点

面情報と凸性を使用しない。頂点座標は有限でなければならない。
