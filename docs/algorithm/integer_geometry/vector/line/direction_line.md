---
title: Line Direction Vector (直線方向ベクトル)
documentation_of: ../../../../../src/algorithm/integer_geometry/vector/line/direction_line.hpp
---

直線を定義する 2 点の差から方向ベクトルを返す。

## API

- `direction(line)`: `line.b - line.a` を返す。

## 時間計算量・空間計算量

- `direction(line)`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した計算量は有理数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。
