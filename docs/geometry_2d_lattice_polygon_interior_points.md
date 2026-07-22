---
title: Lattice Polygon Interior Points (格子多角形の内部格子点数)
documentation_of: ../src/algorithm/geometry/2d/scalar/lattice_polygon_interior_points.hpp
---

## API

`__uint128_t lattice_polygon_interior_points(polygon)`

## 引数

3頂点以上の単純格子多角形を渡す。

## 戻り値

Pickの定理から求めた内部格子点数。

## API別の時間計算量・空間計算量

- `lattice_polygon_interior_points`: 頂点数 $N$ として時間 $O(N\log C)$、追加領域 $O(1)$。

## 注意点

面積0またはPickの定理と整合しない入力では `invalid_argument`。結果が `__uint128_t` を超える場合は `overflow_error`。
