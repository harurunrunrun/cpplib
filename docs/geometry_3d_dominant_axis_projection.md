---
title: Dominant Axis Projection 3D (三次元支配軸射影)
documentation_of: ../src/algorithm/geometry/3d/point/dominant_axis_projection.hpp
---

## API

- `dominant_axis(normal)`: 絶対値最大の法線成分に対応する軸を返す。
- `dominant_axis_projection(point, axis)`: 指定軸を除去して2次元へ射影する。

## API別の時間計算量・空間計算量

- 各APIとも時間・追加領域 $O(1)$。

## 注意点

零法線に対する`dominant_axis`は`std::invalid_argument`を送出する。
