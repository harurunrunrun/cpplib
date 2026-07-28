---
title: Exact Line Intersection (厳密直線交点)
documentation_of: ../../../../../src/algorithm/integer_geometry/result/line_line/line_intersection.hpp
---

非平行な二無限直線の交点を有理座標で返す。

## API

- `line_intersection(first, second)`: 一意な交点を返し、平行なら `nullopt` を返す。

## API別の時間計算量・空間計算量

- `line_intersection(first, second)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
