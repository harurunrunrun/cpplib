---
title: Point on 3D Ray (三次元半直線上判定)
documentation_of: ../src/algorithm/geometry/3d/predicate/on_ray.hpp
---

## API

- `on_ray(ray, point)`: 点が始点を含む半直線上にあるか判定する。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化半直線には `std::invalid_argument` を送出する。
