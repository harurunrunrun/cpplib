---
title: AABB Branch-and-Bound 3D Point Set Diameter (AABB分枝限定による三次元点集合直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/point_set_diameter_3d_aabb_branch_and_bound.hpp
---

軸平行境界箱（AABB）どうしの距離の上界で候補となる点集合の対を枝刈りし、点集合の直径（最遠点対）を厳密に求める。

## API

- `point_set_diameter_3d_aabb_branch_and_bound(points)`: 最大距離と両端のインデックスを `std::optional<PointSetDiameter3DResult>` で返す。同距離の対が複数あれば、インデックス対が辞書順最小のものを選ぶ。点が 2 個未満なら `std::nullopt` を返す。

## API別の時間計算量・空間計算量

点数を $N$、訪れた AABB ノード対の数を $R$、葉で比較した点対の数を $C$ とする。AABB 木の構築は平均 $O(N \log N)$、探索は $O(R + C)$、追加空間は $O(N)$。最悪時間は $O(N^2)$ だが、離れたクラスタや内点が多い入力では候補を大きく減らせる。

## 注意点

- 近似は行わない。AABB による上界は、丸めで過小評価にならないよう大きい側へ広げて計算する。
- 座標は有限でなければならず、違反した場合は `std::invalid_argument` を送出する。
- 真の直径が `long double` の最大値を超える場合、返す `distance` は正の無限大になる。
