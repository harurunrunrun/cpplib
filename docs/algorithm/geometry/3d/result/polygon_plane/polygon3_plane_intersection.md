---
title: Polygon3 Plane Intersection (三次元多角形と平面の交差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polygon_plane/polygon3_plane_intersection.hpp
---

三次元空間内の平面多角形と平面の共通部分を求める。

## API

- `polygon3_plane_intersection(polygon, plane)`: 単純な平面多角形と平面の共通部分を `Polygon3PlaneIntersection3{coplanar_polygon, points, segments}` で返す。2 平面が一致する場合は `coplanar_polygon` に多角形そのものを、交わる場合は交線と多角形の共通部分を `points`, `segments` に入れる。平行で交わらない場合は空の結果を返す。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。

- 2 平面が平行な場合: 時間 $O(N)$、追加空間 $O(1)$（戻り値を除く）。
- 交線が存在する場合: 多角形と交線のイベントをソートするため、最悪時間 $O(N \log N)$、追加空間 $O(N)$。

## 注意点

- 2 平面が一致するときは、面積を持つ多角形をそのまま `coplanar_polygon` に返す。
- 非凸多角形では、共通部分が複数の線分になることがある。
- 頂点数 3 未満または頂点が共面でない多角形には `std::invalid_argument` を送出する。
