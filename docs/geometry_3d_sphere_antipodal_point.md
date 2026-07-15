---
title: Antipodal Point on 3D Sphere (三次元球面上の反対点)
documentation_of: ../src/algorithm/geometry/3d/sphere_antipodal_point.hpp
---

## API

- `sphere_antipodal_point(sphere, point)`: 球の中心に関して反対側の球面点を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

負半径または球面上にない点には `std::invalid_argument` を送出する。半径0では中心自身を返す。
