---
title: Exact Convex Geometry Aggregates (厳密凸幾何集約)
documentation_of: ../../../../../src/algorithm/integer_geometry/aggregate/all/convex.hpp
---

凸包、凸性判定、Minkowski 和の API と、多角形の API を一括で読み込む。

## API

このヘッダ自体は関数を定義せず、次のヘッダを読み込む。

- `point/point_point/add_points.hpp`
- `point_set/point_set/convex_hull.hpp`
- `point_set/point_set_point_set/minkowski_sum.hpp`
- `predicate/point_set/is_convex.hpp`
- `aggregate/all/polygon.hpp（多角形の集約ヘッダ）`

## API別の時間計算量・空間計算量

- 集約ヘッダ自体: 実行時の処理はなく、追加領域は $O(1)$。

## 注意点

計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
