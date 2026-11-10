---
title: Exact 2D Geometry Types (厳密二次元幾何型)
documentation_of: ../../../../../src/algorithm/integer_geometry/type/definition/geometry_types.hpp
---

有理数座標のベクトル・点・直線・線分と、線分交差・点の内外判定の結果を表す列挙型を定義する。

## API

- `Vector`: 成分 `x`, `y`（`Rational`）を持つ。加減算、単項マイナス、スカラー倍・スカラー除算、等値比較を持つ。
- `Point`: 座標 `x`, `y`（`Rational`）を持つ。点とベクトルの加減算、点同士の差（`Vector`）、等値比較、$x$ 座標優先・次に $y$ 座標の辞書式順序 `operator<=>` を持つ。
- `Line`: 直線上の 2 点 `a`, `b` を保持する。
- `Segment`: 線分の端点 `a`, `b` を保持する。
- `SegmentIntersectionKind`: `none`, `touch`, `proper`, `overlap` のいずれか。
- `PointLocation`: `outside`, `boundary`, `inside` のいずれか。

## 時間計算量・空間計算量

- 各型の構築・演算・比較: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した計算量は有理数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。
