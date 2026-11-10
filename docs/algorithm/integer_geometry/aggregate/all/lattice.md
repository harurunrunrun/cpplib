---
title: Exact Lattice Geometry Aggregates (厳密格子幾何集約)
documentation_of: ../../../../../src/algorithm/integer_geometry/aggregate/all/lattice.hpp
---

格子多角形の面積・境界格子点数・内部格子点数の API と、格子点から有理点への変換を一括で読み込む。

## API

このヘッダ自体は関数を定義せず、次のヘッダを読み込む。

- `integer/integer_point_set/lattice_polygon_metrics.hpp`
- `point/integer_point/rational_point.hpp`
- `result/integer_point_set/lattice_point_count.hpp`
- `aggregate/all/types.hpp（型の集約ヘッダ）`

## API別の時間計算量・空間計算量

- 集約ヘッダ自体: 実行時の処理はなく、追加領域は $O(1)$。

## 注意点

計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
