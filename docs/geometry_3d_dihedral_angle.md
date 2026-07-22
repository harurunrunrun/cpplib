---
title: 3D Dihedral Angle (三次元二面角)
documentation_of: ../src/algorithm/geometry/3d/scalar/dihedral_angle.hpp
---

## API

- `dihedral_angle(first, second)`: 向き付き法線間の二面角を $[0,\pi]$ で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

法線を反転すると補角に変わる。零法線には `std::invalid_argument` を送出する。
