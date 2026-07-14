---
title: Convex Polygon Intersection Test (凸多角形同士の交差判定)
documentation_of: ../src/algorithm/geometry/2d/intersect_convex_polygons.hpp
---

## API

- `intersect_convex_polygons(first, second)`: 2閉凸集合が共通点を持つか判定する。

## 引数

`first`, `second` は弱凸な周回列である。

## 戻り値

内部の重なり、辺の交差、接触のいずれかがあれば `true`。

## API別の時間計算量・空間計算量

頂点数を $N,M$ として時間・領域 $O(N+M)$。

## 注意点

空集合との交差は `false`。非凸入力には `std::invalid_argument` を送出する。
