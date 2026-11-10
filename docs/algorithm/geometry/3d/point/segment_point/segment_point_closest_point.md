---
title: Segment-Point Closest Point (線分上の最近点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/segment_point/segment_point_closest_point.hpp
---

## API

- `closest_point(segment, point)`: 閉線分上で `point` に最も近い点を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 退化線分（`segment.a == segment.b`）も 1 点として扱う。
- 座標は有限値でなければならず、違反時は `std::invalid_argument` を送出する。
- 結果を局所座標で表現できない場合は `std::overflow_error` を送出することがある。
