---
title: Intersection of Three 3D Planes (三次元の三平面の共通部分)
documentation_of: ../src/algorithm/geometry/3d/point/three_plane_intersection.hpp
---

## API

- `ThreePlaneIntersection3`: 空、点、直線、平面を表すvariant。
- `three_plane_intersection(first, second, third)`: 三平面の共通部分を返す。

## API別の時間計算量・空間計算量

- 構築・判別・関数実行はいずれも時間・追加領域 $O(1)$。

## 注意点

零法線には `std::invalid_argument` を送出する。一致平面や、2平面が一致して交線が残る場合も型を失わず返す。
