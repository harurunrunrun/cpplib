---
title: Polygon3 Plane Intersection Result (三次元多角形と平面の交差結果型)
documentation_of: ../../../../src/algorithm/geometry/3d/polygon3_plane_intersection3.hpp
---

## API

- `Polygon3PlaneIntersection3::coplanar_polygon`: 平面が多角形の平面と一致するときの多角形。
- `Polygon3PlaneIntersection3::points`: 非共面時の孤立交点。
- `Polygon3PlaneIntersection3::segments`: 非共面時の共通閉線分。
- `empty()`: 3つのfieldがすべて空なら `true` を返す。

## API別の時間計算量・空間計算量

- `empty()`: 時間・追加領域 $O(1)$。
- 構築・fieldへのアクセス: 保持するvector・多角形の要素数を除き時間・追加領域 $O(1)$。

## 注意点

`coplanar_polygon` が値を持つ結果では、`points` と `segments` は空になる。型自体はこの不変条件を検査しない。
