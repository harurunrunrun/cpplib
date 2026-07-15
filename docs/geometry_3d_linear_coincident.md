---
title: 3D Linear Coincidence Test (三次元線形図形の一致判定)
documentation_of: ../src/algorithm/geometry/3d/linear_coincident.hpp
---

## API

- `coincident(Line3, Line3)`: 無限直線が集合として一致するかを返す。
- `coincident(Ray3, Ray3)`: 始点と向きが一致するかを返す。
- `coincident(Segment3, Segment3)`: 端点順を無視して閉線分が一致するかを返す。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

退化直線・半直線には `std::invalid_argument` を送出する。退化線分同士には対応する。
