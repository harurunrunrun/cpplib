---
title: Three-Plane Intersection Result (3平面の共通部分結果型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/plane/three_plane_intersection_result3.hpp
---

3平面すべてに共通する集合を表す結果型を提供します。

## API

- `ThreePlaneIntersection3`: 空集合を `std::monostate`、一点を `Point3`、直線を `Line3`、平面を `Plane3` で表す `std::variant` です。

## API別の時間計算量・空間計算量

型の構築・代入・variant判定は時間・追加空間ともに $O(1)$ です。

## 注意点

このheaderは結果型だけを提供します。共通部分の計算には `three_plane_intersection_planes.hpp` をincludeしてください。