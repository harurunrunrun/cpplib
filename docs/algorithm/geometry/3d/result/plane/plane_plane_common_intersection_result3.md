---
title: Plane-Plane Common Intersection Result (2平面の共通部分結果型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/plane/plane_plane_common_intersection_result3.hpp
---

2 平面の共通部分を表す結果型を提供する。

## API

- `PlanePlaneIntersection3`: `std::variant<std::monostate, Line3, Plane3>` の別名。空集合を `std::monostate`、交線を `Line3`、一致する平面を `Plane3` で表す。

## API別の時間計算量・空間計算量

型の構築・代入・保持している型の判定は、時間・追加空間ともに $O(1)$。

## 注意点

このヘッダは結果型だけを提供する。共通部分を計算するには `plane_plane_common_intersection_planes.hpp` をインクルードすること。
