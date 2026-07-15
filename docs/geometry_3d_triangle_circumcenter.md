---
title: 3D Triangle Circumcenter (三次元三角形の外心)
documentation_of: ../src/algorithm/geometry/3d/triangle_circumcenter.hpp
---

## API

- `triangle_circumcenter(triangle)`: 三角形の平面上にある外心を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化三角形には `std::invalid_argument` を送出する。
