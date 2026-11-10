---
title: Exact Point on Segment Predicate (厳密点線分包含判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/segment_point/on_segment.hpp
---

点が閉線分上にあるかを誤差なく判定する。

## API

- `bool on_segment(const Segment& segment, const Point& point)`: 点が線分上（両端点を含む）にあれば `true` を返す。

## API別の時間計算量・空間計算量

- `on_segment(segment, point)`: 時間 $O(1)$、追加領域 $O(1)$

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
