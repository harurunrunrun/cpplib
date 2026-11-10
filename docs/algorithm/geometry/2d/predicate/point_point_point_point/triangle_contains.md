---
title: Inclusive Point in Triangle (三角形の内部・境界判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/point_point_point_point/triangle_contains.hpp
---

## API

`bool triangle_contains(first, second, third, point)`

## 引数

三角形の 3 頂点 `first`, `second`, `third` と、判定する点 `point` を渡す。

## 戻り値

点が三角形の内部または境界上にあるとき `true`。

## API別の時間計算量・空間計算量

- `triangle_contains`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 頂点の順序は時計回り・反時計回りのどちらでもよい。
- 退化三角形では、3 辺の和集合の上にあるかを判定する。
- 辺上の判定と、点が辺のどちら側にあるかの符号には、辺の長さで正規化した有向距離に対する `GEOMETRY_EPS` を用いる。三角形自体が退化しているかは、辺同士のなす角で判定する。
