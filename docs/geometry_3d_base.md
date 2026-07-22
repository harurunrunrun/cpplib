---
title: 3D Geometry Base Types (三次元幾何の基本型)
documentation_of: ../src/algorithm/geometry/3d/core/geometry_primitives.hpp
---

## API

- `GEOMETRY3D_EPS`, `GEOMETRY3D_PI`: 許容誤差と円周率。
- `Point3{x,y,z}`: 構築、単項符号、加減算、scalar倍・除算、複合代入、辞書順比較、誤差付き等値比較を持つ。
- `Line3`, `Ray3`, `Segment3`, `Plane3`, `Circle3`, `Sphere3`, `Triangle3`, `Tetrahedron3`: 各三次元図形のaggregate型。
- `operator*(scalar, point)`: 左からのスカラー倍。

## API別の時間計算量・空間計算量

- 全構築、演算、比較、field・定数参照: 時間・追加領域 $O(1)$。

## 注意点

`Point3::operator==` は絶対誤差付きだが `operator<` は厳密な辞書順である。方向、法線、半径などの有効性は利用する個別APIが検査する。
