---
title: 3D Line Intersection (三次元直線同士の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/line_line/line_line_intersection.hpp
---

## API

- `line_line_intersection(first, second)`: 2 直線の共通部分を `LinearIntersection3` で返す。結果は空集合（`std::monostate`）、点（`Point3`）、一致する直線（`Line3`、`first` そのもの）のいずれかである。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 平行で異なる直線と、ねじれの位置にある直線では空集合を返す。
- 交差する場合は、両直線上の最近点の距離が許容誤差の範囲で 0 であるときに、その 2 点の中点を返す。
- 退化した直線には `std::invalid_argument` を送出する。
