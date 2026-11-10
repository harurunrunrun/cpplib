---
title: Exact Point Orientation (厳密三点方向判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/integer/point_point_point/orientation.hpp
---

3 点が反時計回り・一直線上・時計回りのどれであるかを符号で判定する。

## API

- `int orientation(const Point& first, const Point& second, const Point& third)`: 反時計回りなら $1$、一直線上なら $0$、時計回りなら $-1$ を返す。
- `int ccw(const Point& first, const Point& second, const Point& third)`: `orientation` と同じ結果を返す別名。

## API別の時間計算量・空間計算量

- `orientation(first, second, third)`: 時間 $O(1)$、追加領域 $O(1)$
- `ccw(first, second, third)`: 時間 $O(1)$、追加領域 $O(1)$

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
