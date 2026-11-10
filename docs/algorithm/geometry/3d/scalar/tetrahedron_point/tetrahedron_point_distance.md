---
title: Point to 3D Tetrahedron Distance (点と三次元四面体の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/tetrahedron_point/tetrahedron_point_distance.hpp
---

## API

- `distance(tetrahedron, point)`: 点 `point` と閉四面体（内部を含む）の集合間距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 点が四面体の内部または境界上にあれば $0$ を返す。
- 退化した四面体には `std::invalid_argument` を送出する。
