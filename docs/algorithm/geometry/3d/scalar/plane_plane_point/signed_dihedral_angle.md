---
title: Signed 3D Dihedral Angle (三次元符号付き二面角)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/plane_plane_point/signed_dihedral_angle.hpp
---

## API

- `signed_dihedral_angle(first, second, oriented_axis)`: 軸 `oriented_axis` まわりの右ねじの向きを正として、第 1 平面の法線から第 2 平面の法線へ回る角を $[-\pi, \pi]$ の範囲（ラジアン）で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 軸は非零で、両平面に平行（両法線と許容誤差内で直交）でなければならない。違反時や、法線が零ベクトルの場合は `std::invalid_argument` を送出する。
