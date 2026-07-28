---
title: 3D Plane-Plane Common Intersection (3次元2平面の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/plane_plane_common_intersection.hpp
---

2平面の共通部分結果型と計算関数をまとめて読み込む互換aggregatorです。

## API

- `PlanePlaneIntersection3`: 空集合・直線・平面のvariantです。
- `plane_plane_common_intersection(first, second)`: 2平面の共通部分を返します。

## API別の時間計算量・空間計算量

型操作と計算関数はいずれも時間・追加空間 $O(1)$ です。

## 注意点

従来の一括include用headerです。結果型と計算関数は個別leafからも利用できます。入力平面は有限で、法線は非零でなければなりません。