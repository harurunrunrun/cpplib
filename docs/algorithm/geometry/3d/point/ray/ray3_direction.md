---
title: 3D Ray Direction (三次元半直線の方向)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/ray/ray3_direction.hpp
---

## API

- `ray3_direction(ray)`: 方向ベクトル `ray.through - ray.origin` を退化検査したうえで返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 方向ベクトルの長さが `GEOMETRY3D_EPS`（$10^{-10}$） 以下なら退化半直線とみなし、`std::invalid_argument` を送出する（絶対許容誤差による判定）。
- 戻り値は単位化されない。
