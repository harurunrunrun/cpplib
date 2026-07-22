---
title: Convex Polyhedron Diameter (凸多面体の直径)
documentation_of: ../src/algorithm/geometry/3d/scalar/convex_polyhedron_diameter.hpp
---

凸多面体のEuclidean直径を、直径を達成する頂点対とともに求める。

## API

- `convex_polyhedron_diameter(polyhedron)`: 全頂点対を調べ、最遠頂点対の添字と距離を
  返す。頂点が2個未満なら `std::nullopt`。
- `convex_polyhedron_diameter_aabb_branch_and_bound(polyhedron)`: 頂点集合のAABB木を
  作り、現在の直径を超えられない部分木対を枝刈りして同じ結果を正確に返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `convex_polyhedron_diameter(polyhedron)` | $O(V^2)$ | $O(1)$ |
| `convex_polyhedron_diameter_aabb_branch_and_bound(polyhedron)` | 平均前処理 $O(V\log V)$ と探索 $O(R+C)$、最悪 $O(V^2)$ | $O(V)$ |

$R$ は訪れたAABBノード対数、$C$ は葉で実比較した頂点対数である。細長い形状や
空間的に分離した頂点群では枝刈りが効くが、全頂点が直径候補になり得る形状では
最悪二乗時間になる。

## 注意点

- 凸多面体の直径は頂点対で達成されるため、面や辺の内部を別に調べる必要はない。
- 同距離ならindex対が辞書順最小のものを返す。
- 非有限頂点を拒否し、`std::invalid_argument` を送出する。
- AABB版も近似を行わず、単純版と同じ直径とindex対を返す。
