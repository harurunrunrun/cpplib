---
title: Perpendicular Bisector (垂直二等分線)
documentation_of: ../src/algorithm/geometry/2d/perpendicular_bisector.hpp
---

## API

`Line perpendicular_bisector(first, second)`

## 引数

異なる2点を渡す。

## 戻り値

中点を `a` とし、線分方向を90度反時計回りに回した方向上の点を `b` とする直線。

## API別の時間計算量・空間計算量

- `perpendicular_bisector`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

2点が誤差付きで一致する場合は `invalid_argument`。
