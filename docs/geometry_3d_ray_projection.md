---
title: 3D Ray Projection (三次元半直線への射影)
documentation_of: ../src/algorithm/geometry/3d/ray_projection.hpp
---

## API

- `projection(ray, point)`: 閉半直線上で点に最も近い射影点を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化半直線には `std::invalid_argument` を送出する。直交射影が始点より後方なら始点を返す。
