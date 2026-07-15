---
title: 3D Planar Intersection Result (三次元平面図形の共通部分型)
documentation_of: ../src/algorithm/geometry/3d/planar_intersection3.hpp
---

## API

- `PlanarIntersection3`: `std::variant<std::monostate, Point3, Segment3, Polygon3>`。空、点、線分、面積を持つ平面多角形を表す。

## API別の時間計算量・空間計算量

- 構築・判別は $O(1)$、多角形のcopyは頂点数を $K$ として時間・領域 $O(K)$。

## 注意点

`std::get_if` または `std::holds_alternative` で結果の次元を安全に判別する。
