---
title: Closest Points between 3D Line and Ray (三次元直線と半直線の最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/line_ray_closest_points.hpp
---

## API

- `closest_points(first, second)`: `Line3` と `Ray3` 上の最近点の対を、引数と同じ順の `std::pair<Point3, Point3>` で返す。引数の型の順序を入れ替えたオーバーロードも提供する。
- この互換用集約ヘッダは `(Line3, Ray3)` 版と `(Ray3, Line3)` 版の両方の個別ヘッダを提供する。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 最近点対が複数ある場合は、そのうち 1 組を返す。
- 退化した直線・半直線、非有限な入力には `std::invalid_argument` を送出する。
- 共通の基準点からの局所座標とパラメータ区間で解き、結果を表現できない場合は `std::overflow_error` を送出する。
