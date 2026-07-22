---
title: 3D Ray Direction (三次元半直線方向)
documentation_of: ../src/algorithm/geometry/3d/point/ray3_direction.hpp
---

## API

- `ray3_direction(ray)`: `ray.through - ray.origin` を有効性検査して返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化半直線には `std::invalid_argument` を送出する。戻り値は単位化されない。
