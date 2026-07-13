---
title: 3D Triangle Normal (三次元三角形の法線)
documentation_of: ../src/algorithm/geometry/geometry_3d/triangle_normal.hpp
---

## API

- `triangle_normal(triangle)`: 頂点順 `a,b,c` に対応する単位法線を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

3頂点が共線な退化三角形には `std::invalid_argument` を送出する。
