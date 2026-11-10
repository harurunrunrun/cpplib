---
title: Exact Lattice Geometry Types (厳密格子幾何型)
documentation_of: ../../../../../src/algorithm/integer_geometry/type/definition/lattice_types.hpp
---

任意精度整数座標の格子点と、格子点数の結果型を定義する。

## API

- `IntegerPoint`: 座標 `x`, `y`（`Integer`、すなわち `ExactInteger`）を保持する。等値比較と、$x$ 座標優先・次に $y$ 座標の辞書式順序 `operator<=>` を持つ。
- `LatticePointCount`: 境界上の格子点数 `boundary` と内部の格子点数 `interior` を保持する。

## 時間計算量・空間計算量

- `IntegerPoint`: 構築・比較とも時間 $O(1)$、追加領域 $O(1)$。
- `LatticePointCount`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

計算は任意精度整数で厳密に行う。記載した計算量は整数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。
