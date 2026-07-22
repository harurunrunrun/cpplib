---
title: 3D Orthogonality (3次元直交判定)
documentation_of: ../src/algorithm/geometry/3d/predicate/orthogonal.hpp
---

## API

- `orthogonal(left, right)`: 2 ベクトルが相対許容誤差 `GEOMETRY3D_EPS` の範囲で直交すれば `true` を返す。各ベクトルを独立にスケーリングし、積和演算で内積を評価する。

## API別の時間計算量・空間計算量

- `orthogonal(left, right)`: 時間 $O(1)$、追加空間 $O(1)$。

## 注意点

- どちらかがゼロベクトルなら `true` を返す。
- NaN または無限大を含む入力には `std::invalid_argument` を送出する。
- 正の有限スカラー倍に対して判定結果は不変である。
