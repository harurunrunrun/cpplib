---
title: Exact Point on Line Predicate (厳密点直線包含判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/line_point/on_line.hpp
---

点が無限に延びる直線上にあるかを誤差なく判定する。

## API

- `bool on_line(const Line& line, const Point& point)`: 点が直線上にあれば `true` を返す。

## API別の時間計算量・空間計算量

- `on_line(line, point)`: 時間 $O(1)$、追加領域 $O(1)$

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
- 直線は相異なる 2 点で表す。`line.a == line.b` の場合は常に `true` になる。
