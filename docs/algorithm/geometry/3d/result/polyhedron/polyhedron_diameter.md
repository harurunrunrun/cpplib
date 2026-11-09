---
title: General Polyhedron Diameter (一般多面体の直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron/polyhedron_diameter.hpp
---

凸性を仮定せず、頂点集合の最遠点対を AABB 分枝限定法（`point_set_diameter_3d`）で求める。多面体領域の直径は頂点対で達成される。

## API

- `polyhedron_diameter(polyhedron)`: 2 頂点のインデックスと距離を `std::optional<PointSetDiameter3DResult>` で返す。頂点が 2 個未満なら `std::nullopt` を返す。同距離の対が複数あれば、インデックス対が辞書順最小のものを返す。

## API別の時間計算量・空間計算量

頂点数を $V$ とすると、構築の時間計算量は平均 $O(V \log V)$。探索は通常は枝刈りされ、最悪時間計算量は $O(V^2)$。追加空間計算量は $O(V)$。

## 注意点

- 面の情報と凸性は使用しない。
- 頂点座標は有限でなければならず、違反した場合は `std::invalid_argument` を送出する。
