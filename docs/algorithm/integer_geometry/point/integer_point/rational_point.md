---
title: Lattice to Rational Point Conversion (格子点有理点変換)
documentation_of: ../../../../../src/algorithm/integer_geometry/point/integer_point/rational_point.hpp
---

整数座標の格子点を、同じ位置を表す有理座標の点に変換する。

## API

- `Point rational_point(const IntegerPoint& point)`: 分母が 1 の有理座標の点を返す。

## API別の時間計算量・空間計算量

- `rational_point(point)`: 時間 $O(1)$、追加領域 $O(1)$

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
