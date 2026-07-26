---
title: Exact 2D Geometry Types (厳密二次元幾何型)
documentation_of: ../../../src/integer_geometry/2d/types.hpp
---

有理数座標のベクトル・点と、直線・線分、判定結果の列挙型を定義する。

## API

- `Vector{x, y}`: ベクトル。加減算、符号反転、スカラー乗除算、等値比較を提供する。
- `Point{x, y}`: 点。辞書順比較、ベクトルとの加減算、2点の差を提供する。
- `Line{a, b}`, `Segment{a, b}`: 無限直線と閉線分を別型で表す。
- `direction(line_or_segment)`: 始点から終点への方向ベクトルを返す。
- `vector_from(from, to)`: `from` から `to` へのベクトルを返す。
- `SegmentIntersectionKind`: `none`, `touch`, `proper`, `overlap`。
- `PointLocation`: `outside`, `boundary`, `inside`。

## API別の時間計算量・空間計算量

座標の最大bit長を $L$、有理数演算時間を $R(L)$ とする。

- 構築、field参照、列挙値: 時間 $O(1)$、座標保存領域 $O(L)$。
- ベクトル・点の演算、比較、`direction`、`vector_from`: 時間 $O(R(L))$、追加領域 $O(L)$。

## 注意点

`Line` と `Segment` は別型である。除数0のベクトル除算は有理数の `std::domain_error` を送出する。
