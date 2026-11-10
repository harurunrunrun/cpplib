---
title: Exact Geometry Type Aggregates (厳密幾何型集約)
documentation_of: ../../../../../src/algorithm/integer_geometry/aggregate/all/types.hpp
---

厳密二次元幾何で使う有理数（`Rational`）、点（`Point`）、ベクトル（`Vector`）、直線（`Line`）、線分（`Segment`）、格子点（`IntegerPoint`）などの型を一括で読み込む。

## API

このヘッダ自体は関数を定義せず、次のヘッダを読み込む。

- `type/definition/geometry_types.hpp`
- `type/definition/lattice_types.hpp`
- `type/definition/rational.hpp`

## API別の時間計算量・空間計算量

- 集約ヘッダ自体: 実行時の処理はなく、追加領域は $O(1)$。

## 注意点

計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
