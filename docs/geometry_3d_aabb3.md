---
title: 3D Axis-Aligned Bounding Box (三次元軸平行境界箱)
documentation_of: ../src/algorithm/geometry/3d/aabb3.hpp
---

`AABB3` は閉区間
`[minimum.x, maximum.x] * [minimum.y, maximum.y] * [minimum.z, maximum.z]`
で表す軸平行境界箱である。kd-tree、Octree、triangle BVH の枝刈りにも使う。

## API

- `AABB3{minimum, maximum}`: 2隅から境界箱を構築する。
- `box.valid()`: 両端が有限で、全軸で `minimum <= maximum` なら `true`。
- `box.contains(point)`: 閉じた境界を含め、点が箱内にあるか判定する。
- `box.intersects(other)`: 境界で接する場合を含め、2箱が交差するか判定する。
- `box.distance(point)`: 点と箱のEuclidean距離を返す。箱内なら0。
- `box.require_valid()`: 不正な箱なら `std::invalid_argument` を送出する。

## API別の時間計算量・空間計算量

すべてのAPIは時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- `contains`、`intersects`、`distance` は箱を検査し、不正なら
  `std::invalid_argument` を送出する。点も有限でなければならない。
- 境界を含む閉集合として扱う。
- 距離は `std::hypot` で計算し、平方和による不要なoverflowを避ける。
