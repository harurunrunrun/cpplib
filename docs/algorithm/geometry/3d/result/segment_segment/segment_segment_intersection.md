---
title: 3D Segment Intersection (三次元線分同士の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/segment_segment/segment_segment_intersection.hpp
---

2 本の三次元線分の共通部分を求める。

## API

- `segment_segment_intersection(first, second)`: 共通部分を `LinearIntersection3` で返す。空集合（`std::monostate`）、点（`Point3`）、線分（`Segment3`）のいずれかである。

## API別の時間計算量・空間計算量

- 時間・追加空間ともに $O(1)$。

## 注意点

- 退化した線分（両端点がほぼ一致）は、一方だけの場合も両方の場合も一点として処理する。両方が退化している場合は、代表点 `a` どうしが完全に一致するときだけ共通部分を持つ。
- 端点だけで接する場合は点を、正の長さを持つ重なりは線分を返す。
