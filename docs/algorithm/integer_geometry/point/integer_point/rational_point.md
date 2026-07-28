---
title: Lattice to Rational Point Conversion (格子点有理点変換)
documentation_of: ../../../../../src/algorithm/integer_geometry/point/integer_point/rational_point.hpp
---

整数座標の格子点を同値な有理座標点へ変換する。

## API

- `rational_point(point)`: 分母1の有理座標点を返す。

## API別の時間計算量・空間計算量

- `rational_point(point)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
