---
title: Dominant-Axis Projection to Point (支配軸除去による二次元点射影)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/point/dominant_axis_projection_to_point.hpp
---

## API

- `dominant_axis_projection(point, axis)`: `Point3` から `axis` が指す成分を除き、二次元の `Point` を返す。成分の対応は次の通り。
  - `DominantAxis3::x`: $(y, z)$
  - `DominantAxis3::y`: $(z, x)$
  - `DominantAxis3::z`: $(x, y)$

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- `axis` には有効な `DominantAxis3` を与える（`x`, `y` 以外の値はすべて `z` として扱われる）。
- この関数は法線から軸を選ばない。必要なら `dominant_axis_from_point3.hpp` を併用する。
