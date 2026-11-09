---
title: Ray-Point Closest Point (半直線上の最近点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/ray_point/ray_point_closest_point.hpp
---

## API

- `closest_point(ray, point)`: 閉半直線上で `point` に最も近い点を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 半直線は退化しておらず（`ray.origin != ray.through`）、座標は有限値でなければならない。違反時は `std::invalid_argument` を送出する。
- 直交射影が始点より後方にある場合は始点を返す。
- 結果を局所座標で表現できない場合は `std::overflow_error` を送出することがある。
