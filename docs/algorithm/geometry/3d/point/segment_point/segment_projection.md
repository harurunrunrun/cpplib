---
title: 3D Segment Projection (三次元線分への射影)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/segment_point/segment_projection.hpp
---

## API

- `projection(segment, point)`: 閉線分上で `point` に最も近い点を返す。`closest_point(segment, point)` の別名である。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 直交射影の位置が線分の外にある場合は、近い方の端点を返す（媒介変数を $[0, 1]$ に制限するのと同じ）。
- 退化線分では、その 1 点を返す。
