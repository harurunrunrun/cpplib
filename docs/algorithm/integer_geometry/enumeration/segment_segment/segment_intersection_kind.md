---
title: Exact Segment Intersection Kind (厳密線分交差種別)
documentation_of: ../../../../../src/algorithm/integer_geometry/enumeration/segment_segment/segment_intersection_kind.hpp
---

2 つの閉線分の関係を、交差なし・接触・真の交差・重なりに分類する。

## API

- `SegmentIntersectionKind segment_intersection_kind(const Segment& first, const Segment& second)`: 交差の種類を返す。`none` は共有点なし、`touch` はちょうど 1 点を共有し、それがいずれかの線分の端点である場合、`proper` は互いの内部で 1 点のみ交わる場合、`overlap` は同一直線上で長さ正の区間を共有する場合である。

## API別の時間計算量・空間計算量

- `segment_intersection_kind(first, second)`: 時間 $O(1)$、追加領域 $O(1)$

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
