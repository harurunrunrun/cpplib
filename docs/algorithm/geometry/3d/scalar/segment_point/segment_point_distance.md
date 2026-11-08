---
title: Segment-Point Distance (線分と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/segment_point/segment_point_distance.hpp
---

## API

- `distance(segment, point)`: 閉線分 `segment` と点 `point` の最短距離を `long double` で返す。

## API別の時間計算量・空間計算量

- `distance`: 時間・追加領域 $O(1)$。

## 注意点

- 退化した線分（両端点が一致する線分）は一点として扱う。
- 座標は有限値でなければならず、違反時は `std::invalid_argument` を送出する。
- 座標差を正規化してから距離を計算するため、中間計算でのオーバーフローは起きない。距離が `long double` で表現できない場合は `std::overflow_error` を送出する。
