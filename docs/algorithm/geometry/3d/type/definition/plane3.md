---
title: 3D Plane Type (三次元平面型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/plane3.hpp
---

## API

- `Plane3{point, normal}`: 平面上の一点と法線で平面を表すaggregate型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域 $O(1)$。

## 注意点

法線は零ベクトルであってはならない。単位長である必要はない。
