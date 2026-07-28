---
title: Exact Convexity Predicate (厳密凸性判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/point_set/is_convex.hpp
---

巡回順に与えた多角形が凸かを判定する。

## API

- `is_convex(polygon, strict)`: 凸なら真を返す。`strict` が真なら一直線の連続辺も許さない。

## API別の時間計算量・空間計算量

- `is_convex(polygon, strict)`: 時間 O(n)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
