---
title: 3D Triangle Incenter (三次元三角形の内心)
documentation_of: ../src/algorithm/geometry/3d/triangle_incenter.hpp
---

## API

- `triangle_incenter(triangle)`: 三辺から等距離にある内心を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化三角形には `std::invalid_argument` を送出する。
