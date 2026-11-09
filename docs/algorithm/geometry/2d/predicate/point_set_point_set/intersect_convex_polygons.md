---
title: Convex Polygon Intersection Test (凸多角形同士の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/point_set_point_set/intersect_convex_polygons.hpp
---

## API

- `bool intersect_convex_polygons(first, second)`: 2 つの閉凸集合が共通点を持つか判定する。

## 引数

`first`, `second` は弱凸な周回列（`std::vector<Point>`、値渡し）である。

## 戻り値

内部の重なり、辺の交差、接触のいずれかがあれば `true`。

## API別の時間計算量・空間計算量

- `intersect_convex_polygons`: 頂点数を $N, M$ として時間・領域 $O(N + M)$。

## 注意点

- ミンコフスキー差による最小距離を求め、それが `GEOMETRY_EPS` 以下なら交差すると判定する。
- 空集合との交差は `false` を返す。凸でない入力には `std::invalid_argument` を送出する。
