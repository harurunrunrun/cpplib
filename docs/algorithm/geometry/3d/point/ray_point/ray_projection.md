---
title: 3D Ray Projection (三次元半直線への射影)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/ray_point/ray_projection.hpp
---

## API

- `projection(ray, point)`: 閉半直線上で `point` に最も近い点を返す。`closest_point(ray, point)` の別名である。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 退化半直線や非有限入力には `std::invalid_argument` を送出する。
- 直交射影が始点より後方にある場合は始点を返す。
