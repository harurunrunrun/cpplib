---
title: Convex Polyhedron Diameter (凸多面体の直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron/convex_polyhedron_diameter_default.hpp
---

凸多面体の直径（最遠頂点対）を求める。実装は頂点集合に対して `point_set_diameter_3d`（厳密な AABB 分枝限定法）を呼ぶ。

## API

- `convex_polyhedron_diameter(polyhedron)`: ユークリッド直径を達成する頂点の添字対と距離を `std::optional<PointSetDiameter3DResult>` で返す。頂点が 2 個未満なら `std::nullopt` を返す。

## API別の時間計算量・空間計算量

頂点数を $V$ とすると、前処理は平均 $O(V \log V)$、探索は通常は大きく枝刈りされ、最悪時間は $O(V^2)$。追加空間は $O(V)$。

## 注意点

- 凸多面体の直径は頂点対で達成される。同距離の対が複数あれば、添字対が辞書順最小のものを返す。
- 非有限な頂点には `std::invalid_argument` を送出する。
