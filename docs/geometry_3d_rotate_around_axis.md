---
title: 3D Rotation Around Axis (三次元軸回転)
documentation_of: ../src/algorithm/geometry/geometry_3d/rotate_around_axis.hpp
---

## API

- `rotate_around_axis(point, axis, theta)`: 原点を通る `axis` 周りに `point` を `theta` ラジアン回転する。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

右手系のRodriguesの公式を用いる。零の `axis` には `std::invalid_argument` を送出する。
