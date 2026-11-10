---
title: Angle between 3D Lines (三次元直線同士の角度)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/line_line/line_line_angle.hpp
---

## API

- `line_line_angle(first, second)`: 向きを無視した 2 直線のなす小さい方の角を $[0, \pi/2]$ の範囲（ラジアン）で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 退化した直線（2 点が許容誤差内で一致する）には `std::invalid_argument` を送出する。
