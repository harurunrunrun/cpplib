---
title: Exact Point Addition (厳密点加算)
documentation_of: ../../../../../src/algorithm/integer_geometry/point/point_point/add_points.hpp
---

2 点の座標ごとの和を返す。

## API

- `Point add_points(const Point& first, const Point& second)`: Minkowski 和などで使う、座標ごとの和を返す。

## API別の時間計算量・空間計算量

- `add_points(first, second)`: 時間 $O(1)$、追加領域 $O(1)$

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
