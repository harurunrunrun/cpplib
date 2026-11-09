---
title: Exact Squared Point Distance (厳密点間距離二乗)
documentation_of: ../../../../../src/algorithm/integer_geometry/scalar/point_point/squared_distance.hpp
---

2 点間のユークリッド距離の二乗を返す。

## API

- `squared_distance(first, second)`: 距離の二乗を `Rational` で返す。

## 時間計算量・空間計算量

- `squared_distance(first, second)`: 時間 (1)$、追加領域 (1)$。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した計算量は有理数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。
