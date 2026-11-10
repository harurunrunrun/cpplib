---
title: 3D Orthogonality (三次元直交判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/point_point/orthogonal.hpp
---

## API

- `orthogonal(left, right)`: 2 つのベクトルが相対許容誤差 `GEOMETRY3D_EPS` の範囲で直交すれば `true` を返す。各ベクトルを最大絶対成分で独立にスケーリングし、`std::fma` による積和で内積を評価する。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- どちらかがゼロベクトルなら `true` を返す。
- NaN または無限大を含む入力には `std::invalid_argument` を送出する。
- 正の有限なスカラー倍に対して、判定結果は不変である。
