---
title: Sphere Relation Result (球の位置関係結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/sphere_relation_result.hpp
---

2 つの球面の位置関係を表す列挙型 `SphereRelation`。

## API

- `SPHERE_INSIDE`（0）: 一方の球面が他方の球の内部にあり、接しない。
- `SPHERE_INTERNALLY_TANGENT`（1）: 内接する。
- `SPHERE_INTERSECT`（2）: 2 つの球面が円で交わる。
- `SPHERE_EXTERNALLY_TANGENT`（3）: 外接する。
- `SPHERE_SEPARATE`（4）: 互いに離れている。
- `SPHERE_COINCIDENT`（5）: 中心と半径がともに等しい。

## API別の時間計算量・空間計算量

- 構築・比較・代入: 時間・追加領域ともに $O(1)$。

## 注意点

`SPHERE_INSIDE` は引数の順序を区別せず、どちらの球が内側かという情報は保持しない。
