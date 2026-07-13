---
title: 3D Reflection (三次元鏡映)
documentation_of: ../src/algorithm/geometry/geometry_3d/reflection.hpp
---

## API

- `reflection(line, point)`: 点を直線の周りに180度回転した点を返す。
- `reflection(plane, point)`: 点を平面に関して鏡映した点を返す。

## API別の時間計算量・空間計算量

- 各overload: 時間・追加領域 $O(1)$。

## 注意点

退化直線または零法線の平面には `std::invalid_argument` を送出する。
