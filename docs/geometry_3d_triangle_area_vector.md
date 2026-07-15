---
title: 3D Triangle Area Vector (三次元三角形の向き付き面積ベクトル)
documentation_of: ../src/algorithm/geometry/3d/triangle_area_vector.hpp
---

## API

- `triangle_area_vector(triangle)`: 頂点順に従う法線方向を持ち、長さが面積に等しいベクトルを返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

頂点順を反転すると符号が反転する。退化三角形では零ベクトルを返す。
