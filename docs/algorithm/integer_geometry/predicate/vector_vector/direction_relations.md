---
title: Exact Vector Direction Predicates (厳密ベクトル方向判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/vector_vector/direction_relations.hpp
---

2 本のベクトルが平行か、直交かを判定する。

## API

- `parallel(first, second)`: 外積が $0$ なら `true` を返す。零ベクトルはどのベクトルとも平行と判定される。
- `orthogonal(first, second)`: 内積が $0$ なら `true` を返す。零ベクトルはどのベクトルとも直交と判定される。

## 時間計算量・空間計算量

- `parallel`, `orthogonal`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した計算量は有理数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。
