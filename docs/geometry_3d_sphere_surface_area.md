---
title: Sphere Surface Area (球の表面積)
documentation_of: ../src/algorithm/geometry/geometry_3d/sphere_surface_area.hpp
---

## API

- `sphere_surface_area(sphere)`: $4\pi r^2$ を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

負半径には `std::invalid_argument` を送出する。
