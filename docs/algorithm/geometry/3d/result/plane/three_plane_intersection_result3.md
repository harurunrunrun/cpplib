---
title: Three-Plane Intersection Result (3平面の共通部分結果型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/plane/three_plane_intersection_result3.hpp
---

3 平面すべてに共通する集合を表す結果型を提供する。

## API

- `ThreePlaneIntersection3`: `std::variant<std::monostate, Point3, Line3, Plane3>` の別名。空集合を `std::monostate`、一点を `Point3`、直線を `Line3`、平面を `Plane3` で表す。

## API別の時間計算量・空間計算量

型の構築・代入・保持している型の判定は、時間・追加空間ともに $O(1)$。

## 注意点

このヘッダは結果型だけを提供する。共通部分を計算するには `three_plane_intersection_planes.hpp` をインクルードすること。
