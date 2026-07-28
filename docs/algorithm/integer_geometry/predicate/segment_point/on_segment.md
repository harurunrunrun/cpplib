---
title: Exact Point on Segment Predicate (厳密点線分包含判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/segment_point/on_segment.hpp
---

点が閉線分上にあるかを誤差なく判定する。

## API

- `on_segment(segment, point)`: 点が線分上なら真を返す。

## API別の時間計算量・空間計算量

- `on_segment(segment, point)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
