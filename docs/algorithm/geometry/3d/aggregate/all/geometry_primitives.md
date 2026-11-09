---
title: 3D Geometry Base Types (三次元幾何の基本型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/geometry_primitives.hpp
---

三次元幾何の定数と基本図形の型をまとめて include する集約ヘッダである。

## API

- `GEOMETRY3D_EPS`, `GEOMETRY3D_PI`: 許容誤差と円周率。
- `Point3{x, y, z}`: 構築、単項の符号、加減算、スカラー倍・スカラーでの除算、複合代入、辞書順比較、誤差付きの等値比較を持つ。
- `Line3`, `Ray3`, `Segment3`, `Plane3`, `Circle3`, `Sphere3`, `Triangle3`, `Tetrahedron3`: 各三次元図形の集成体型。
- `operator*(scalar, point)`: 左からのスカラー倍。

## API 別の時間計算量・空間計算量

- すべての構築・演算・比較、メンバ・定数の参照: 時間・追加空間ともに $O(1)$。

## 注意点

- `Point3::operator==` は各座標の差の絶対値が `GEOMETRY3D_EPS` 以下かで判定する（絶対誤差付き）が、`operator<` は厳密な辞書順である。したがって、両者は整合しない場合がある。
- 方向・法線・半径などの有効性は、それを利用する個別の API が検査する。
