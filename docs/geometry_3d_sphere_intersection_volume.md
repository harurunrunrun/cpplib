---
title: Sphere Intersection Volume (球の共通部分体積)
documentation_of: ../src/algorithm/geometry/geometry_3d/sphere_intersection_volume.hpp
---

## API

- `sphere_intersection_volume(first, second)`: 2閉球の共通部分体積を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

離隔、接触、内包、同心・一致に対応する。負半径には `std::invalid_argument` を送出する。
