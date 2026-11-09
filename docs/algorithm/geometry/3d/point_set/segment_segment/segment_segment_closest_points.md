---
title: Segment-Segment Closest Points (二線分の最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/segment_segment/segment_segment_closest_points.hpp
---

## API

- `closest_points(first, second)`: 2 本の閉線分の間の距離を達成する点対を、引数と同じ順の `std::pair<Point3, Point3>` で返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 退化した線分（両端点が一致）は 1 点として扱う。
- 最近点対が複数ある場合は、そのうち 1 組を返す。
- 非有限な入力には `std::invalid_argument` を、最近点が `long double` で表現できない場合は `std::overflow_error` を送出する。
