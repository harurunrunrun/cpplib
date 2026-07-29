---
title: 3D Point Set Diameter via Deterministic Convex Hull (決定的凸包経由の三次元点集合直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/point_set_diameter_3d_via_convex_hull_default.hpp
---

決定的な分割統治法で凸包を構築し、極点だけをAABB分枝限定探索して
点集合直径を返す。

## API

- `point_set_diameter_3d_via_convex_hull(points)`: 最大距離と元入力での
  両端indexを `PointSetDiameter3DResult` で返す。同距離ならindex対が
  辞書順最小のものを選び、点が2個未満なら `std::nullopt`。

## API別の時間計算量・空間計算量

入力点数を $N$、凸包極点数を $H$、AABB探索量を $R+C$ とする。
最悪時間は $O(N\log N+H\log H+R+C)$、追加領域は $O(N+H)$。
$R+C$ は最悪 $O(H^2)$ である。

## 注意点

全点同一・共線・共面入力も扱い、元indexの辞書順tieを保つ。
座標は有限でなければならず、違反時は `std::invalid_argument`。
距離が正の無限大へ丸められる場合だけ、tieを保つため全点のAABB探索へ戻る。
