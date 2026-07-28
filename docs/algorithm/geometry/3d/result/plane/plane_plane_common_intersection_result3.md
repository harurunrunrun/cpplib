---
title: Plane-Plane Common Intersection Result (2平面の共通部分結果型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/plane/plane_plane_common_intersection_result3.hpp
---

2平面の共通部分を表す結果型を提供します。

## API

- `PlanePlaneIntersection3`: 空集合を `std::monostate`、交線を `Line3`、一致平面を `Plane3` で表す `std::variant` です。

## API別の時間計算量・空間計算量

型の構築・代入・variant判定は時間・追加空間ともに $O(1)$ です。

## 注意点

このheaderは結果型だけを提供します。共通部分の計算には `plane_plane_common_intersection_planes.hpp` をincludeしてください。