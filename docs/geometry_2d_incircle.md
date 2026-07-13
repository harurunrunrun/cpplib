---
title: 2D Incircle (二次元内接円)
documentation_of: ../src/algorithm/geometry/2d/incircle.hpp
---

## API

- `incircle(first, second, third)`: 三角形の内接円を返す。

## API別の時間計算量・空間計算量

- `incircle`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化三角形には `std::invalid_argument` を送出する。
