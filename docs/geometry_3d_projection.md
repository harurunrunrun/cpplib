---
title: 3D Projection (三次元射影)
documentation_of: ../src/algorithm/geometry/3d/projection.hpp
---

## API

- `projection(line, point)`: 点を無限直線へ正射影する。
- `projection(plane, point)`: 点を平面へ正射影する。

## API別の時間計算量・空間計算量

- 各overload: 時間・追加領域 $O(1)$。

## 注意点

退化直線または零法線の平面には `std::invalid_argument` を送出する。
