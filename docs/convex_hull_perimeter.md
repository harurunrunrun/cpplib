---
title: Convex Hull Perimeter (凸包周長)
documentation_of: ../src/algorithm/geometry/2d/scalar/convex_hull_perimeter.hpp
---

## API

- `convex_hull_perimeter(points)`: 点集合の凸包境界長を返す。

## 引数

`points` は重複を含んでもよい点集合である。

## 戻り値

凸包の周長を `long double` で返す。

## API別の時間計算量・空間計算量

点数を $N$ として時間 $O(N\log N)$、領域 $O(N)$。

## 注意点

空集合・1点は0、線分へ退化した凸包は線分長の2倍を返す。
