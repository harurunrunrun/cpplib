---
title: 3D Plane Type (三次元平面型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/plane3.hpp
---

## API

- `Plane3{point, normal}`: 平面上の 1 点 `point` と法線 `normal` で平面を表す集成体型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域 $O(1)$。

## 注意点

- 法線は零ベクトルであってはならない（型自体は検査せず、各 API が検査する）。単位ベクトルである必要はない。
