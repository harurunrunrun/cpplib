---
title: Exact Oriented Area Product (厳密有向面積積)
documentation_of: ../../../../../src/algorithm/integer_geometry/scalar/point_point_point/cross_points.hpp
---

基準点から 2 点へ向かう 2 本のベクトルの外積を返す。

## API

- `cross(origin, first, second)`: `cross(first - origin, second - origin)` を返す。正なら `origin`, `first`, `second` は反時計回り、負なら時計回り、/bin/bash$ なら共線である。

## 時間計算量・空間計算量

- `cross(origin, first, second)`: 時間 (1)$、追加領域 (1)$。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した計算量は有理数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。
