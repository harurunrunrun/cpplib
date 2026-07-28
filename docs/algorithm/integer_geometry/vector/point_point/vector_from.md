---
title: Vector Between Points (二点間ベクトル)
documentation_of: ../../../../../src/algorithm/integer_geometry/vector/point_point/vector_from.hpp
---

始点から終点へのベクトルを返す。

## API

- `vector_from(from, to)`: `to - from` を返す。

## API別の時間計算量・空間計算量

- `vector_from(from, to)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
