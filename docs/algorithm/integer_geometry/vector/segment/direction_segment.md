---
title: Segment Direction Vector (線分方向ベクトル)
documentation_of: ../../../../../src/algorithm/integer_geometry/vector/segment/direction_segment.hpp
---

線分の終点から始点を引いた方向ベクトルを返す。

## API

- `direction(segment)`: `segment.b - segment.a` を返す。

## 時間計算量・空間計算量

- `direction(segment)`: 時間 (1)$、追加領域 (1)$。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した計算量は有理数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。
