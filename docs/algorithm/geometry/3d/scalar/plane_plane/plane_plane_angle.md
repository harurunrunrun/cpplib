---
title: Angle between 3D Planes (三次元平面同士の角度)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/plane_plane/plane_plane_angle.hpp
---

## API

- `plane_plane_angle(first, second)`: 法線の向きを無視した 2 平面のなす小さい方の角を $[0, \pi/2]$ の範囲（ラジアン）で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 法線が零ベクトル、または非有限の場合は `std::invalid_argument` を送出する。
