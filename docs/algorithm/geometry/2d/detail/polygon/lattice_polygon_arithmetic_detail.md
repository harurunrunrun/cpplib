---
title: Lattice Polygon Detail (格子多角形の内部処理)
documentation_of: ../../../../../../src/algorithm/geometry/2d/detail/polygon/lattice_polygon_arithmetic_detail.hpp
---

## API

名前空間 `lattice_polygon_detail` の内部関数群。`Wide` は多倍長整数 `ExactInteger` の別名である。

- `unsigned_difference(first, second)`: 整数座標 2 つの差の絶対値を `__uint128_t` で返す。
- `gcd(first, second)`: `__uint128_t` 同士の最大公約数をユークリッドの互除法で返す。
- `signed_double_area(polygon)`: `IntegerPoint<Coordinate>` の列が表す多角形の符号付き面積の 2 倍を `Wide` で返す。
- `to_uint128(value)`: `Wide` の値を `__uint128_t` に変換する。

## 引数

`Coordinate` は `std::integral` を満たす整数型。`to_uint128` は `Wide` を受け取る。

## 戻り値

座標差の絶対値、最大公約数、符号付き面積の 2 倍、`__uint128_t` への変換結果を返す。

## API別の時間計算量・空間計算量

- `unsigned_difference`, `to_uint128`: 時間 $O(1)$、領域 $O(1)$。
- `gcd`: 時間 $O(\log V)$（$V$ は引数の大きさ、128 bit なので高々定数回）、領域 $O(1)$。
- `signed_double_area`: 頂点数を $N$ として時間 $O(N)$、追加領域 $O(1)$（多倍長整数の内部領域を除く）。

## 注意点

公開 API から利用する内部実装である。`to_uint128` は値が負、または `__uint128_t` の範囲を超える場合に `std::overflow_error` を送出する。
