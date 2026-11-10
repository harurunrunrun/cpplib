---
title: Exact Segment Direction Predicates (厳密線分方向判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/segment_segment/direction_relations.hpp
---

2 線分の方向が平行か・直交するかを判定する。

## API

- `bool parallel(const Segment& first, const Segment& second)`: 方向が平行なら `true` を返す。
- `bool orthogonal(const Segment& first, const Segment& second)`: 方向が直交するなら `true` を返す。

## API別の時間計算量・空間計算量

- `parallel(Segment, Segment)`: 時間 $O(1)$、追加領域 $O(1)$
- `orthogonal(Segment, Segment)`: 時間 $O(1)$、追加領域 $O(1)$

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
- 長さ $0$ の線分では方向ベクトルが零になり、結果は意味を持たない。
