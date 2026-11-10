---
title: Closest Points between 3D Ray and Plane (三次元半直線と平面の最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/ray_plane_closest_points.hpp
---

## API

- `closest_points(first, second)`: `Ray3` と `Plane3` 上の最近点の対を、引数と同じ順の `std::pair<Point3, Point3>` で返す。引数の型の順序を入れ替えたオーバーロードも提供する。
- この互換用集約ヘッダは `(Ray3, Plane3)` 版と `(Plane3, Ray3)` 版の両方の個別ヘッダを提供する。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 最近点対が複数ある場合は、そのうち 1 組を返す。
- 退化した半直線、法線が零ベクトルの平面、非有限な入力には `std::invalid_argument` を送出する。
