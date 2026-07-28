---
title: Exact Point on Line Predicate (厳密点直線包含判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/line_point/on_line.hpp
---

点が無限直線上にあるかを誤差なく判定する。

## API

- `on_line(line, point)`: 点が直線上なら真を返す。

## API別の時間計算量・空間計算量

- `on_line(line, point)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
