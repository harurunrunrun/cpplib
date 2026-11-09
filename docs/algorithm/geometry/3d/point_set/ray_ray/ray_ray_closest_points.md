---
title: Closest Points between 3D Ray and Ray (三次元半直線同士の最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/ray_ray/ray_ray_closest_points.hpp
---

## API

- `closest_points(first, second)`: 2 本の `Ray3` 上の最近点を、引数と同じ順の `std::pair<Point3, Point3>` で返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 最近点対が複数ある場合は、そのうち 1 組を返す。
- 退化した半直線、非有限な入力には `std::invalid_argument` を送出する。
- 共通の基準点からの局所座標とパラメータ区間で解く。結果が `long double` で表現できない場合は `std::overflow_error` を送出する。
