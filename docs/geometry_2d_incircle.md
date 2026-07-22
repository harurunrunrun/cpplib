---
title: 2D Incircle (二次元内接円) [CGL_7_B]
documentation_of: ../src/algorithm/geometry/2d/shape/incircle.hpp
---

## API

- `incircle(first, second, third)`: 三角形の内接円を返す。

## API別の時間計算量・空間計算量

- `incircle`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化三角形には `std::invalid_argument` を送出する。半径は1辺の単位方向と内心への差分の外積から求め、辺ベクトル同士の積を避ける。
