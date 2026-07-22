---
title: Lattice Polygon Detail (格子多角形内部処理)
documentation_of: ../src/algorithm/geometry/2d/detail/lattice_polygon_arithmetic_detail.hpp
---

## API

`lattice_polygon_detail` 名前空間の内部関数群。

## 引数

整数座標または固定256 bit整数を受け取る。

## 戻り値

座標差、最大公約数、符号付き面積2倍、`__uint128_t` 変換結果を返す。

## API別の時間計算量・空間計算量

- `unsigned_difference`, `gcd`, `to_uint128`: 時間 $O(1)$、領域 $O(1)$。
- `signed_double_area`: 頂点数 $N$ として時間 $O(N)$、領域 $O(1)$。

## 注意点

公開APIから利用する内部実装である。範囲外変換では `overflow_error`。
