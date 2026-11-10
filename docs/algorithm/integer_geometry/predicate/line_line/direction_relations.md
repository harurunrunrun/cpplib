---
title: Exact Line Direction Predicates (厳密直線方向判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/line_line/direction_relations.hpp
---

2 直線が平行か・直交するかを、方向ベクトルから判定する。

## API

- `bool parallel(const Line& first, const Line& second)`: 2 直線が平行（同一直線を含む）なら `true` を返す。
- `bool orthogonal(const Line& first, const Line& second)`: 2 直線の方向が直交するなら `true` を返す。

## API別の時間計算量・空間計算量

- `parallel(Line, Line)`: 時間 $O(1)$、追加領域 $O(1)$
- `orthogonal(Line, Line)`: 時間 $O(1)$、追加領域 $O(1)$

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
- 直線は相異なる 2 点 `a`, `b` で表す。`a == b` の退化した直線では方向ベクトルが零になり、結果は意味を持たない。
