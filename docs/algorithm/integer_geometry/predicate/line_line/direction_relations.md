---
title: Exact Line Direction Predicates (厳密直線方向判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/line_line/direction_relations.hpp
---

二直線の平行・直交を方向ベクトルから判定する。

## API

- `parallel(Line, Line)`: 二直線が平行なら真を返す。
- `orthogonal(Line, Line)`: 二直線が直交する方向なら真を返す。

## API別の時間計算量・空間計算量

- `parallel(Line, Line)`: 時間 O(1)、追加領域 O(1)。
- `orthogonal(Line, Line)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
