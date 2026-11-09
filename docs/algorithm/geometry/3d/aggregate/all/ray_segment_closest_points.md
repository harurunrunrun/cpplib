---
title: Closest Points between 3D Ray and Segment (三次元半直線と線分の最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/ray_segment_closest_points.hpp
---

## API

- `closest_points(first, second)`: `Ray3` 上と `Segment3` 上の最近点を、引数と同じ順の `std::pair<Point3, Point3>` で返す。異なる型の組なので、引数を逆順にしたオーバーロードも提供する。
- この互換用の集約ヘッダは、`ray_segment` 順と `segment_ray` 順の両方の個別ヘッダを読み込む。

## API別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加領域ともに $O(1)$。

## 注意点

最近点対が複数ある場合（平行な場合など）は、そのうち 1 組を返す。線分は退化（両端点が一致）していてもよい。退化した半直線・非有限な入力には `std::invalid_argument` を送出する。
内部では共通の基準点からの局所座標とパラメータ区間で解き、結果の座標を `long double` で表現できない場合は `std::overflow_error` を送出する。
