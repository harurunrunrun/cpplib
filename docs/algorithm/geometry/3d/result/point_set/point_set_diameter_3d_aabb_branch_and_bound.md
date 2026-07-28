---
title: AABB Branch-and-Bound 3D Point Set Diameter (AABB分枝限定による三次元点集合直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/point_set_diameter_3d_aabb_branch_and_bound.hpp
---

AABB間の厳密な距離上界で候補集合対を枝刈りし、点集合直径を求める。

## API

- `point_set_diameter_3d_aabb_branch_and_bound(points)`: 最大距離と
  両端indexを `PointSetDiameter3DResult` で返す。同距離ならindex対が
  辞書順最小の結果を選び、点が2個未満なら `std::nullopt`。

## API別の時間計算量・空間計算量

点数を $N$、訪れたAABBノード対を $R$、葉で比較した点対を $C$ とする。
平均的な前処理は $O(N\log N)$、探索は $O(R+C)$、追加領域は $O(N)$。
最悪時間は $O(N^2)$。離れたclusterや内点が多い入力では候補を大きく減らせる。

## 注意点

近似は行わない。AABB上界は丸めで過小にならないよう正方向へ広げる。
座標は有限でなければならず、違反時は `std::invalid_argument`。
真の直径が `long double` 上限を超える場合、返す `distance` は
正の無限大となる。
