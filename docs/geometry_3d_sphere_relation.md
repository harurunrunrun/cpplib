---
title: 3D Sphere Relation (三次元球の位置関係)
documentation_of: ../src/algorithm/geometry/3d/predicate/sphere_relation.hpp
---

## API

- `sphere_relation(first, second)`: 球面同士を `SPHERE_INSIDE`, `SPHERE_INTERNALLY_TANGENT`, `SPHERE_INTERSECT`, `SPHERE_EXTERNALLY_TANGENT`, `SPHERE_SEPARATE`, `SPHERE_COINCIDENT` に分類する。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

Sphere3 は中心・半径が有限かつ半径が非負でなければならず、違反時は std::invalid_argument を送出する。

`SPHERE_INSIDE` は一方の球面が他方の球体内部にあり接しない状態で、引数順に依存しない。負半径には `std::invalid_argument` を送出する。
