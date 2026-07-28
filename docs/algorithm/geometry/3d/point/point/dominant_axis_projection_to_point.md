---
title: Dominant-Axis Projection to Point (支配軸除去による二次元点射影)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/point/dominant_axis_projection_to_point.hpp
---

## API

- `dominant_axis_projection(point, axis)`: `Point3` から指定軸の成分を除き、二次元の `Point` を返す。
  - `x`: $(y,z)$
  - `y`: $(z,x)$
  - `z`: $(x,y)$

## API別の時間計算量・空間計算量

- `dominant_axis_projection`: 時間・追加領域 $O(1)$。

## 注意点

`axis` には有効な `DominantAxis3` を与える。この関数は法線から軸を選ばないため、必要なら `dominant_axis_from_point3.hpp` を併用する。
