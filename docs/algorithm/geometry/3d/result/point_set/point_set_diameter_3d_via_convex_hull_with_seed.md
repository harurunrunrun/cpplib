---
title: 3D Point Set Diameter via Convex Hull with Seed (シード指定凸包経由の三次元点集合直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/point_set_diameter_3d_via_convex_hull_with_seed.hpp
---

指定したシードで乱択凸包（`convex_hull_3d_with_seed`）を構築し、その極点だけを対象に AABB 分枝限定探索を行って点集合の直径を返す。

## API

- `point_set_diameter_3d_via_convex_hull_with_seed(points, random_seed)`: 最大距離と、元の入力における両端のインデックスを `std::optional<PointSetDiameter3DResult>` で返す。`random_seed` は凸包の増分構築の順序だけを変える。点が 2 個未満なら `std::nullopt` を返す。

## API別の時間計算量・空間計算量

入力の点数を $N$、凸包の極点数を $H$、AABB 探索の手間を $R + C$ とする。シードを入力と独立に選び、距離が有限であれば、期待時間 $O(N \log N + H \log H + R + C)$、追加空間 $O(N + H)$。最悪時間は $O(N^2 \log N + H^2)$、最悪追加空間は $O(N^2 + H)$。

## 注意点

- シードは直径の値や、同距離のときの辞書順規則には影響しない。
- 全点が同一・共線・共面の入力も扱う。
- 座標は有限でなければならず、違反した場合は `std::invalid_argument` を送出する。
- 距離が正の無限大に丸められる場合に限り、全点に対する探索に切り替える。
