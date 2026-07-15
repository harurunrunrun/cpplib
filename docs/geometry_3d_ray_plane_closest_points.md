---
title: Closest Points between 3D Ray and Plane (三次元半直線と平面の最近点対)
documentation_of: ../src/algorithm/geometry/3d/ray_plane_closest_points.hpp
---

## API

- `closest_points(first, second)`: `Ray3` と `Plane3` 上の最近点を引数と同じ順のpairで返す。異種型では逆順overloadも提供する。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

最近点対が複数ある場合はそのうち1組を返す。退化した直線・半直線または零法線の平面には `std::invalid_argument` を送出し、退化線分には対応する。
