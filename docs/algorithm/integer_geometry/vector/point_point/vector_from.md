---
title: Vector Between Points (二点間ベクトル)
documentation_of: ../../../../../src/algorithm/integer_geometry/vector/point_point/vector_from.hpp
---

始点から終点へ向かうベクトルを返す。

## API

- `vector_from(from, to)`: `to - from` を返す。

## 時間計算量・空間計算量

- `vector_from(from, to)`: 時間 (1)$、追加領域 (1)$。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した計算量は有理数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。
