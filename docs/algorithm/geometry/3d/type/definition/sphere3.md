---
title: 3D Sphere Type (三次元球型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/sphere3.hpp
---

## API

- `Sphere3{center, radius}`: 中心 `center`（`Point3`）と半径 `radius`（`long double`）で球を表す集成体型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域ともに $O(1)$。

## 注意点

半径は非負でなければならない。型自体は検査しないので、必要なら `geometry3d_validate(sphere)` を使う。
