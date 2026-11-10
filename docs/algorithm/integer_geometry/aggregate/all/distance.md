---
title: Exact Geometry Distance Aggregates (厳密幾何距離集約)
documentation_of: ../../../../../src/algorithm/integer_geometry/aggregate/all/distance.hpp
---

二乗距離・二乗ノルム、点集合の二乗距離指標（最近点対、直径）の厳密な API と、凸幾何の API を一括で読み込む。

## API

このヘッダ自体は関数を定義せず、次のヘッダを読み込む。

- `result/point_set/point_set_squared_metrics.hpp`
- `scalar/point_point/squared_distance.hpp`
- `scalar/vector/squared_norm.hpp`
- `aggregate/all/convex.hpp（凸幾何の集約ヘッダ）`

## API別の時間計算量・空間計算量

- 集約ヘッダ自体: 実行時の処理はなく、追加領域は $O(1)$。

## 注意点

計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
