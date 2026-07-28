---
title: Exact Segment Direction Predicates (厳密線分方向判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/segment_segment/direction_relations.hpp
---

二線分の方向が平行・直交かを判定する。

## API

- `parallel(Segment, Segment)`: 方向が平行なら真を返す。
- `orthogonal(Segment, Segment)`: 方向が直交なら真を返す。

## API別の時間計算量・空間計算量

- `parallel(Segment, Segment)`: 時間 O(1)、追加領域 O(1)。
- `orthogonal(Segment, Segment)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
