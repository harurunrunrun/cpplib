---
title: 3D Point Set Diameter (三次元点集合直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/point_set_diameter_3d_default.hpp
---

exact AABB分枝限定により、三次元点集合のEuclidean直径を正確に求める。

## API

- `point_set_diameter_3d(points)`: 最大距離と両端indexを
  `PointSetDiameter3DResult` で返す。同距離ならindex対が辞書順最小の
  結果を選び、点が2個未満なら `std::nullopt`。

## API別の時間計算量・空間計算量

点数を $N$、AABB木の訪問node対数を $K$ とすると、構築は
$O(N\log N)$、探索は $O(K)$、追加領域は $O(N)$。
通常は距離上界で大部分のnode対を捨てるが、最悪時は $K=O(N^2)$ である。

## 注意点

座標は有限でなければならず、違反時は `std::invalid_argument`。
距離比較は座標差を仮数と2進指数に分けてoverflowを避ける。真の距離が
`long double` 上限を超える場合、返す `distance` は正の無限大となる。
AABBの浮動小数点上界は枝刈り候補の選別だけに用い、不確実な比較では
node対を残すため、枝刈りによって正解を失わない。
