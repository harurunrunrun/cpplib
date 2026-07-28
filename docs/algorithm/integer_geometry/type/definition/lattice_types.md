---
title: Exact Lattice Geometry Types (厳密格子幾何型)
documentation_of: ../../../../../src/algorithm/integer_geometry/type/definition/lattice_types.hpp
---

任意精度整数座標の格子点と格子点数の結果型を定義する。

## API

- `IntegerPoint`: 格子点を保持する。
- `LatticePointCount`: 境界点数と内部点数を保持する。

## API別の時間計算量・空間計算量

- `IntegerPoint`: 時間 構築・比較とも O(1)、追加領域 O(1)。
- `LatticePointCount`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
