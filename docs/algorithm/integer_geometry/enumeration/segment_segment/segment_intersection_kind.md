---
title: Exact Segment Intersection Kind (厳密線分交差種別)
documentation_of: ../../../../../src/algorithm/integer_geometry/enumeration/segment_segment/segment_intersection_kind.hpp
---

二閉線分の関係を非交差、接触、真の交差、重なりに分類する。

## API

- `segment_intersection_kind(first, second)`: 線分の交差種別を返す。

## API別の時間計算量・空間計算量

- `segment_intersection_kind(first, second)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
