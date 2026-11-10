---
title: 3D Line and Segment Intersection (三次元直線と線分の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/line_segment/line_segment_intersection.hpp
---

## API

- `line_segment_intersection(line, segment)`: 直線と線分の共通部分を `LinearIntersection3` で返す。結果は空集合（`std::monostate`）、点（`Point3`）、線分（`Segment3`、`segment` そのもの）のいずれかである。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 退化した線分は 1 点として扱う。
- 退化した直線には `std::invalid_argument` を送出する。
