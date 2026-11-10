---
title: Exact Lattice Point Count (厳密格子点数)
documentation_of: ../../../../../src/algorithm/integer_geometry/result/integer_point_set/lattice_point_count.hpp
---

格子多角形（頂点が整数座標の多角形）の境界上の格子点数と内部の格子点数をまとめて返す。

## API

- `lattice_point_count(polygon)`: `std::vector<IntegerPoint>` で与えた多角形について、`LatticePointCount{boundary, interior}` を返す。境界点数は各辺の $\gcd(\lvert \Delta x \rvert, \lvert \Delta y \rvert)$ の和、内部点数は Pick の定理から求める。

## 時間計算量・空間計算量

頂点数を $n$、座標の絶対値の最大値を $C$ とする。

- `lattice_point_count(polygon)`: 時間 $O(n \log C)$、追加領域 $O(1)$。

## 注意点

計算は任意精度整数で厳密に行う。記載した計算量は整数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。

多角形は頂点数 $3$ 以上の単純格子多角形とする。頂点数が $3$ 未満の場合や、面積が $0$ などで Pick の定理が適用できない場合は `std::invalid_argument` を送出する。
