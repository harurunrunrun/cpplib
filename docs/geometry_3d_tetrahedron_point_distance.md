---
title: Point to 3D Tetrahedron Distance (点と三次元四面体の距離)
documentation_of: ../src/algorithm/geometry/3d/tetrahedron_point_distance.hpp
---

## API

- `distance(tetrahedron, point)`: 点と閉四面体の集合間距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

内部または境界なら0を返す。退化四面体には `std::invalid_argument` を送出する。
