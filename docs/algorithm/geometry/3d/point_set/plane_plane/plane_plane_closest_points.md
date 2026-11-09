---
title: Closest Points between 3D Plane and Plane (三次元平面同士の最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/plane_plane/plane_plane_closest_points.hpp
---

## API

- `closest_points(first, second)`: 2 つの `Plane3` 上の最近点を、引数と同じ順の `std::pair` で返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 平行でない場合は、交線（`plane_plane_intersection` の戻り値）の点 `a` を両要素として返す。
- 平行な場合は、`first.point` とその `second` への射影を返す。2 平面が一致する場合も同様である。
- 零法線または非有限の法線を持つ平面には `std::invalid_argument` を送出する。
