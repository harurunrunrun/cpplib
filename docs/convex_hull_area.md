---
title: Convex Hull Area (凸包面積)
documentation_of: ../src/algorithm/geometry/2d/scalar/convex_hull_area.hpp
---

## API

- `convex_hull_area(points)`: 点集合の凸包面積を返す。

## 引数

`points` は重複を含んでもよい点集合である。

## 戻り値

非負の面積を `long double` で返す。

## API別の時間計算量・空間計算量

点数を $N$ として時間 $O(N\log N)$、領域 $O(N)$。

## 注意点

2点以下または全点共線なら0を返す。
