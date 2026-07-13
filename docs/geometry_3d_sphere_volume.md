---
title: Sphere Volume (球の体積)
documentation_of: ../src/algorithm/geometry/geometry_3d/sphere_volume.hpp
---

## API

- `sphere_volume(sphere)`: $4\pi r^3/3$ を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

負半径には `std::invalid_argument` を送出する。
