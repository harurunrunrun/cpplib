---
title: 3D Plane Unit Normal (三次元平面の単位法線)
documentation_of: ../src/algorithm/geometry/3d/plane3_unit_normal.hpp
---

## API

- `plane3_unit_normal(plane)`: 平面の法線を単位化して返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

零法線には `std::invalid_argument` を送出する。向きは入力法線と同じである。
