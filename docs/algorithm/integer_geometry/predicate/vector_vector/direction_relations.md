---
title: Exact Vector Direction Predicates (厳密ベクトル方向判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/vector_vector/direction_relations.hpp
---

二ベクトルの平行・直交を判定する。

## API

- `parallel(Vector, Vector)`: 外積が0かを返す。
- `orthogonal(Vector, Vector)`: 内積が0かを返す。

## API別の時間計算量・空間計算量

- `parallel(Vector, Vector)`: 時間 O(1)、追加領域 O(1)。
- `orthogonal(Vector, Vector)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
