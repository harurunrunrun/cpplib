---
title: Closest Points between 3D Segment and Plane (三次元線分と平面の最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/segment_plane_closest_points.hpp
---

## API

- `closest_points(first, second)`: `Segment3` 上と `Plane3` 上の最近点を、引数と同じ順の `std::pair<Point3, Point3>` で返す。異なる型の組なので、引数を逆順にしたオーバーロードも提供する。
- この互換用の集約ヘッダは、`segment_plane` 順と `plane_segment` 順の両方の個別ヘッダを読み込む。

## API別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加領域ともに $O(1)$。

## 注意点

線分と平面が交わる場合は交点を両方の成分に入れて返す。交わらない場合は、平面に近い方の端点（同距離なら `segment.a`）とその平面への射影を返す。最近点対が複数ある場合は、そのうち 1 組を返す。
線分は退化していてもよい。法線が零ベクトルの平面・非有限な入力には `std::invalid_argument` を送出する。
