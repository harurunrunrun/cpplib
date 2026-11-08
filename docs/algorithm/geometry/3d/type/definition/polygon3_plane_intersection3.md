---
title: Polygon3 Plane Intersection Result (三次元多角形と平面の交差結果型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/polygon3_plane_intersection3.hpp
---

## API

- `Polygon3PlaneIntersection3::coplanar_polygon`: 平面が多角形を含む平面と一致するときの多角形（`std::optional<Polygon3>`）。
- `Polygon3PlaneIntersection3::points`: 共面でない場合の孤立交点の列。
- `Polygon3PlaneIntersection3::segments`: 共面でない場合の共通部分をなす閉線分の列。
- `empty()`: `coplanar_polygon` が値を持たず、`points` と `segments` がともに空なら `true` を返す。

## API別の時間計算量・空間計算量

- `empty()`: 時間・追加領域ともに $O(1)$。
- 構築・各メンバへのアクセス: 保持する `std::vector`・多角形の要素数に比例する分を除き、時間・追加領域ともに $O(1)$。

## 注意点

`coplanar_polygon` が値を持つ結果では、`points` と `segments` は空になる。型自体はこの不変条件を検査しない。
