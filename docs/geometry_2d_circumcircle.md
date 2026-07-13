---
title: 2D Circumcircle (二次元外接円)
documentation_of: ../src/algorithm/geometry/2d/circumcircle.hpp
---

## API

- `circumcircle(first, second, third)`: 3点を通る外接円を返す。

## API別の時間計算量・空間計算量

- `circumcircle`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

3点が共線の場合は `std::invalid_argument` を送出する。
