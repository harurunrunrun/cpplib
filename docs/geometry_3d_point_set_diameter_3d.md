---
title: 3D Point Set Diameter (三次元点集合直径)
documentation_of: ../src/algorithm/geometry/3d/point_set_diameter_3d.hpp
---

三次元点集合のEuclidean直径と、その両端のindexを正確に求める。

## API

- `point_set_diameter_3d(points)`: 最大距離の
  `PointSetDiameter3DResult{first, second, distance}` を返す。`first < second` で、
  同距離ならindex対が辞書順最小のものを返す。点が2個未満なら `nullopt`。

## API別の時間計算量・空間計算量

全ての異なる点対を調べるため時間 $O(N^2)$、追加領域 $O(1)$。
既に凸包頂点列を持つ場合、その頂点列を入力すれば $O(H^2)$ で求められる。

## 注意点

- 3次元には2次元回転calipersの直接対応がないため、安全でない候補削減は行わない。
- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 距離は3引数 `std::hypot` で計算し、平方距離のoverflowを避ける。
