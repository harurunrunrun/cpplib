---
title: Exact Polygon Aggregates (厳密多角形集約)
documentation_of: ../../../../../src/algorithm/integer_geometry/aggregate/all/polygon.hpp
---

多角形の面積、点の内外判定の API と、基本的な判定の API を一括で読み込む。

## API

このヘッダ自体は関数を定義せず、次のヘッダを読み込む。

- `enumeration/point_set_point/point_in_polygon.hpp`
- `scalar/point_set/polygon_area.hpp`
- `aggregate/all/predicates.hpp（判定の集約ヘッダ）`

## API別の時間計算量・空間計算量

- 集約ヘッダ自体: 実行時の処理はなく、追加領域は $O(1)$。

## 注意点

計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
