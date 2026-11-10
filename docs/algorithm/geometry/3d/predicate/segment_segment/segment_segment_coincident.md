---
title: 3D Segment-Segment Coincidence (三次元線分同士の一致判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_segment/segment_segment_coincident.hpp
---

## API

- `coincident(first, second)`: 2 本の `Segment3` の端点が順序を除いて一致するなら `true` を返す。

## API別の時間計算量・空間計算量

- `coincident`: 時間・追加領域ともに $O(1)$。

## 注意点

- 部分的に重なるだけの場合は `false` を返す。
- 端点の一致は `Point3` の `==`（各座標の差が `GEOMETRY3D_EPS` 以下）で判定する。
- 退化した線分同士も、表す点が一致すれば `true` を返す。例外は送出しない。
