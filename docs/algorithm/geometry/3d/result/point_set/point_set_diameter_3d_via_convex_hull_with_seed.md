---
title: 3D Point Set Diameter via Convex Hull with Seed (seed指定凸包経由の三次元点集合直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/point_set_diameter_3d_via_convex_hull_with_seed.hpp
---

指定seedで凸包を構築し、極点だけをAABB分枝限定探索して点集合直径を返す。

## API

- `point_set_diameter_3d_via_convex_hull_with_seed(points, random_seed)`:
  最大距離と元入力での両端indexを返す。seedは凸包の増分順序だけを変える。
  点が2個未満なら `std::nullopt`。

## API別の時間計算量・空間計算量

入力点数を $N$、凸包極点数を $H$、AABB探索量を $R+C$ とする。seedを入力と
独立に選び距離が有限なら期待
$O(N\log N+H\log H+R+C)$ 時間、$O(N+H)$ 追加領域。
最悪時間は $O(N^2\log N+H^2)$、最悪追加領域は $O(N^2+H)$。

## 注意点

seedは直径の意味と同距離時の辞書順規則を変えない。全点同一・共線・共面入力も
扱う。座標は有限でなければならず、違反時は `std::invalid_argument`。
距離が正の無限大へ丸められる場合だけ全点探索へ戻る。
