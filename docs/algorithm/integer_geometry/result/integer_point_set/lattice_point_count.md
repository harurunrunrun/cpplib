---
title: Exact Lattice Point Count (厳密格子点数)
documentation_of: ../../../../../src/algorithm/integer_geometry/result/integer_point_set/lattice_point_count.hpp
---

格子多角形の境界点数と内部点数をまとめて返す。

## API

- `lattice_point_count(polygon)`: `LatticePointCount` を返す。

## API別の時間計算量・空間計算量

- `lattice_point_count(polygon)`: 時間 O(n log C)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。 多角形は頂点数3以上の単純格子多角形とする。
