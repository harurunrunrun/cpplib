---
title: 3D Plane Orthonormal Basis (三次元平面の正規直交基底)
documentation_of: ../src/algorithm/geometry/3d/plane_orthonormal_basis.hpp
---

## API

- `plane_orthonormal_basis(plane)`: 平面上の互いに直交する単位ベクトル2本を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

零法線には `std::invalid_argument` を送出する。法線と返す2本は右手系をなす。
