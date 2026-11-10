---
title: 3D Sphere Relation (三次元球の位置関係)
documentation_of: ../../../../../../src/algorithm/geometry/3d/enumeration/sphere_sphere/sphere_relation.hpp
---

## API

- `sphere_relation(first, second)`: 2 つの球面の位置関係を `SphereRelation` で返す。値は次のいずれかである。
  - `SPHERE_INSIDE`: 一方の球面が他方の球体の内部にあり、接しない。
  - `SPHERE_INTERNALLY_TANGENT`: 内接する。
  - `SPHERE_INTERSECT`: 円で交わる。
  - `SPHERE_EXTERNALLY_TANGENT`: 外接する。
  - `SPHERE_SEPARATE`: 互いに外部にあり、接しない。
  - `SPHERE_COINCIDENT`: 中心と半径が一致する。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

`Sphere3` は中心・半径が有限かつ半径が非負でなければならず、違反時は `std::invalid_argument` を送出する。
判定は中心間距離と半径の最大値をスケールとする相対許容誤差 `GEOMETRY3D_EPS` で行う。結果は引数の順序に依存しない。
