---
title: 3D Point Set Diameter via Deterministic Convex Hull (決定的凸包経由の三次元点集合直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/point_set_diameter_3d_via_convex_hull_default.hpp
---

決定的な分割統治法（`convex_hull_3d`）で凸包を構築し、その極点だけを対象に AABB 分枝限定探索を行って点集合の直径を返す。

## API

- `point_set_diameter_3d_via_convex_hull(points)`: 最大距離と、元の入力における両端のインデックスを `std::optional<PointSetDiameter3DResult>` で返す。同距離の対が複数あれば、インデックス対が辞書順最小のものを選ぶ。点が 2 個未満なら `std::nullopt` を返す。

## API別の時間計算量・空間計算量

入力の点数を $N$、凸包の極点数を $H$、AABB 探索の手間を $R + C$ とすると、最悪時間 $O(N \log N + H \log H + R + C)$、追加空間 $O(N + H)$。$R + C$ は最悪 $O(H^2)$ である。

## 注意点

- 全点が同一・共線・共面の入力も扱い、元のインデックスについての辞書順のタイブレーク規則を保つ。重複点は最小のインデックスで代表する。
- 座標は有限でなければならず、違反した場合は `std::invalid_argument` を送出する。
- 距離が正の無限大に丸められる場合に限り、タイブレーク規則を保つため全点に対する AABB 探索に切り替える。
