---
title: Line Direction Vector (直線方向ベクトル)
documentation_of: ../../../../../src/algorithm/integer_geometry/vector/line/direction_line.hpp
---

直線を定義する二点の差から方向ベクトルを返す。

## API

- `direction(Line)`: 直線の方向ベクトルを返す。

## API別の時間計算量・空間計算量

- `direction(Line)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
