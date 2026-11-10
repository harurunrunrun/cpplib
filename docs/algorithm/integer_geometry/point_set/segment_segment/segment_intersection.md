---
title: Exact Segment Intersection Set (厳密線分交差集合)
documentation_of: ../../../../../src/algorithm/integer_geometry/point_set/segment_segment/segment_intersection.hpp
---

2 つの閉線分の共通部分を、0 点、1 点、または重なり区間の両端点として返す。

## API

- `std::vector<Point> segment_intersection(const Segment& first, const Segment& second)`: 共有点がなければ空列、1 点で交わればその点、重なり区間があればその両端点を辞書順で返す。

## API別の時間計算量・空間計算量

- `segment_intersection(first, second)`: 時間 $O(1)$、追加領域 $O(1)$

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
