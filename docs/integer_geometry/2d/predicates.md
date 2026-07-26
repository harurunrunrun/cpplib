---
title: Exact 2D Predicates (厳密二次元幾何判定)
documentation_of: ../../../src/integer_geometry/2d/predicates.hpp
---

有理数座標に対して誤差なしの向き・所属・平行・直交・線分交差判定を行う。

## API

- `dot(first, second)`, `cross(first, second)`: 内積と外積を返す。
- `cross(origin, first, second)`: 3点の符号付き面積の2倍を返す。
- `orientation(first, second, third)`, `ccw(...)`: 左折なら1、右折なら-1、共線なら0を返す。
- `on_line(line, point)`, `on_segment(segment, point)`: 点の所属を判定する。
- `parallel`, `orthogonal`: Vector、Line、Segment同士の平行・直交を判定する。
- `segment_intersection_kind(first, second)`: 線分の関係を4分類する。
- `segments_intersect(first, second)`: 共通点があれば `true` を返す。

## API別の時間計算量・空間計算量

座標の最大bit長を $L$、有理数演算時間を $R(L)$ とする。

- 全API: 時間 $O(R(L))$、追加領域 $O(L)$。

## 注意点

許容誤差を使わない。零ベクトルは任意のベクトルと平行かつ直交と判定される。退化線分も端点1点の閉集合として交差判定できる。
