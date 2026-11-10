---
title: Strict Point in Triangle (三角形の内部判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/point_point_point_point/triangle_contains_strict.hpp
---

## API

`bool triangle_contains_strict(first, second, third, point)`

## 引数

三角形の 3 頂点 `first`, `second`, `third` と、判定する点 `point` を渡す。

## 戻り値

点が境界を除く内部にあるとき `true`。

## API別の時間計算量・空間計算量

- `triangle_contains_strict`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 頂点の順序は時計回りでもよい。
- 退化三角形では常に `false` を返す。
- 境界の除外と、点が辺のどちら側にあるかの符号には、辺の長さで正規化した有向距離に対する `GEOMETRY_EPS` を用いる。三角形自体が退化しているかは、辺同士のなす角で判定する。
