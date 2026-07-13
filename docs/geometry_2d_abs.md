---
title: 2D Vector Length (二次元ベクトル長)
documentation_of: ../src/algorithm/geometry/2d/abs.hpp
---

## API

- `abs(point)`: ベクトルのユークリッド長を返す。

## API別の時間計算量・空間計算量

- `abs`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

`std::sqrt(norm(point))` を計算し、戻り値は `long double` である。
