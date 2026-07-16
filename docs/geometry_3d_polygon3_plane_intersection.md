---
title: Polygon3 Plane Intersection (三次元多角形と平面の交差)
documentation_of: ../src/algorithm/geometry/3d/polygon3_plane_intersection.hpp
---

## API

- `Polygon3PlaneIntersection3`: 二平面が一致するときの `coplanar_polygon` と、
  それ以外の孤立 `points`・閉区間 `segments` を保持する。`empty()` は共通部分が
  空かを返す。
- `polygon3_plane_intersection(polygon, plane)`: 単純な平面多角形と平面の
  共通部分を返す。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。二平面が平行な場合は時間 $O(N)$、追加領域 $O(1)$。
交線が存在する場合は多角形と直線の event をソートするため、最悪時間 $O(N\log N)$、
追加領域 $O(N)$。`Polygon3PlaneIntersection3::empty()` は時間・領域ともに $O(1)$。

## 注意点

二平面が一致するときは面積を持つ多角形をそのまま `coplanar_polygon` に返す。
非共面の非凸多角形では、共通部分が複数線分になることがある。
