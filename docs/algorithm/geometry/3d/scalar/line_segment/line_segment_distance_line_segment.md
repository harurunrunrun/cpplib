---
title: Distance between 3D Line and Segment in Line-Segment Order (直線・線分順の三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/line_segment/line_segment_distance_line_segment.hpp
---

## API

- `distance(line, segment)`: `Line3` と `Segment3` の集合間距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 交差または重なる場合は $0$ を返す。
- 線分は退化（両端点が一致）していてもよい。
- 退化した直線、非有限な入力には `std::invalid_argument` を、距離が表現できない場合は `std::overflow_error` を送出する。
