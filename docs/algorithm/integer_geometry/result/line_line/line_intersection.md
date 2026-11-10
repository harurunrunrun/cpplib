---
title: Exact Line Intersection (厳密直線交点)
documentation_of: ../../../../../src/algorithm/integer_geometry/result/line_line/line_intersection.hpp
---

2 本の無限直線の交点を有理数座標で返す。

## API

- `line_intersection(first, second)`: 交点がただ 1 つなら `std::optional<Point>` としてその点を返し、平行（一致する場合を含む）なら `std::nullopt` を返す。

## 時間計算量・空間計算量

- `line_intersection(first, second)`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した計算量は有理数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。
