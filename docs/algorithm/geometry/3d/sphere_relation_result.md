---
title: Sphere Relation Result (球の位置関係結果)
documentation_of: ../../../../src/algorithm/geometry/3d/sphere_relation_result.hpp
---

二つの球面の位置関係を表す列挙型。

## API

- `SPHERE_INSIDE`: 一方の球面が他方の球体内部にあり、接しない。
- `SPHERE_INTERNALLY_TANGENT`: 内接する。
- `SPHERE_INTERSECT`: 二つの球面が円で交わる。
- `SPHERE_EXTERNALLY_TANGENT`: 外接する。
- `SPHERE_SEPARATE`: 互いに離れている。
- `SPHERE_COINCIDENT`: 中心と半径が等しい。

## API別の時間計算量・空間計算量

- 構築・比較・代入: 時間・追加領域ともに $O(1)$。

## 注意点

`SPHERE_INSIDE` は引数順を区別しない。どちらの球が内側かを表す情報は保持しない。
