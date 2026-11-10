---
title: 3D Dihedral Angle (三次元二面角)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/plane_plane/dihedral_angle.hpp
---

## API

- `dihedral_angle(first, second)`: 2 平面の向き付き法線のなす角を二面角として $[0, \pi]$ の範囲（ラジアン）で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 法線の向きを考慮するため、一方の法線を反転すると補角（$\pi - \theta$）に変わる。向きを無視した角は `plane_plane_angle` を使う。
- 法線が零ベクトル、または非有限の場合は `std::invalid_argument` を送出する。
