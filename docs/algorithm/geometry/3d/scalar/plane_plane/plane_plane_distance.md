---
title: Distance between 3D Plane and Plane (三次元平面同士の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/plane_plane/plane_plane_distance.hpp
---

## API

- `distance(first, second)`: 2 つの `Plane3` の集合間距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 平行でない（交差する）場合や一致する場合は $0$ を返す。平行な場合は `first.point` と `second` の距離になる。
- 法線が零ベクトルの平面には `std::invalid_argument` を送出する。
