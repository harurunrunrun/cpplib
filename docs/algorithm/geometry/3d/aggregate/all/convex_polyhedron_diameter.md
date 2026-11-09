---
title: Convex Polyhedron Diameter (凸多面体の直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/convex_polyhedron_diameter.hpp
---

凸多面体のユークリッド直径を、直径を達成する頂点対とともに求める。

このヘッダは `convex_polyhedron_diameter_default.hpp` と `convex_polyhedron_diameter_aabb_branch_and_bound.hpp` をまとめて include する、後方互換のための集約ヘッダである。

## API

```cpp
std::optional<PointSetDiameter3DResult> convex_polyhedron_diameter(const ConvexPolyhedron3& polyhedron);
std::optional<PointSetDiameter3DResult> convex_polyhedron_diameter_aabb_branch_and_bound(const ConvexPolyhedron3& polyhedron);
```

- `convex_polyhedron_diameter(polyhedron)`: 頂点集合 `polyhedron.vertices` に `point_set_diameter_3d` を適用し、最遠頂点対の添字 `first`, `second`（`first < second`）と距離 `distance` を返す。頂点が 2 個未満なら `std::nullopt` を返す。
- `convex_polyhedron_diameter_aabb_branch_and_bound(polyhedron)`: 頂点集合の AABB 木を作り、現在の直径を超えられない部分木の対を枝刈りして、厳密な結果を返す。

現在の実装では `point_set_diameter_3d` 自体が AABB 分枝限定法を呼ぶため、2 つの API は同一の処理を行い、同一の結果を返す。

## API 別の時間計算量・空間計算量

頂点数を $V$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `convex_polyhedron_diameter(polyhedron)` | 平均前処理 $O(V \log V)$ と探索 $O(R+C)$、最悪 $O(V^2)$ | $O(V)$ |
| `convex_polyhedron_diameter_aabb_branch_and_bound(polyhedron)` | 平均前処理 $O(V \log V)$ と探索 $O(R+C)$、最悪 $O(V^2)$ | $O(V)$ |

$R$ は訪れた AABB ノード対の数、$C$ は葉で実際に比較した頂点対の数である。細長い形状や空間的に分離した頂点群では枝刈りがよく効くが、すべての頂点が直径の候補になり得る形状では最悪で二乗時間になる。

## 注意点

- 凸多面体の直径は頂点対で達成されるため、面や辺の内部を別に調べる必要はない。
- 距離が等しい対が複数あれば、添字対が辞書順最小のものを返す。
- 非有限な頂点があれば `std::invalid_argument` を送出する。
- 近似は行わない。
