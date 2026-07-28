---
title: Closest Points between 3D Line and Plane (三次元直線と平面の最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/line_plane_closest_points.hpp
---

## API

- `closest_points(first, second)`: `Line3` と `Plane3` 上の最近点を引数と同じ順のpairで返す。異種型では逆順overloadも提供する。
- この互換集約ヘッダは `(Line3, Plane3)` と `(Plane3, Line3)` の両leafを提供する。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

最近点対が複数ある場合はそのうち1組を返す。退化直線または零法線平面には `std::invalid_argument` を送出する。
