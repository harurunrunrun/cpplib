---
title: Polygon3 Plane Intersection (三次元多角形と平面の交差)
documentation_of: ../src/algorithm/geometry/3d/polygon3_plane_intersection.hpp
---

## API

- `Polygon3PlaneIntersection3`: 共面時の `coplanar_polygon`、それ以外の孤立 `points` と `segments` を保持し、`empty()` で空判定する。
- `polygon3_plane_intersection(polygon, plane)`: 一般の単純平面多角形と平面の共通部分を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として時間 $O(N^2)$、領域 $O(N)$。`empty()` は $O(1)$。

## 注意点

共面時は面積を持つ多角形をそのまま返す。非共面の非凸多角形では複数線分になりうる。
