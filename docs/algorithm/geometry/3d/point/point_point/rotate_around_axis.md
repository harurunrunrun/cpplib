---
title: 3D Rotation Around Axis (三次元軸回転)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/point_point/rotate_around_axis.hpp
---

## API

- `rotate_around_axis(point, axis, theta)`: 原点を通り `axis` を方向とする軸の周りに、`point` を `theta` ラジアン回転した点を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 右手系の Rodrigues の回転公式を用いる（`axis` の向きに右ねじが進む向きが正の回転）。
- `axis` は内部で `unit` により単位化される。零ベクトルまたは非有限の `axis` には `std::invalid_argument` を送出する。
