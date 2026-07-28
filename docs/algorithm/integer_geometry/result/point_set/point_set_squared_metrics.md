---
title: Exact Point Set Squared Metrics (厳密点集合距離指標)
documentation_of: ../../../../../src/algorithm/integer_geometry/result/point_set/point_set_squared_metrics.hpp
---

点集合の最近点対距離二乗と直径二乗を計算する。

## API

- `closest_pair_squared(points)`: 2点以上なら最近点対距離二乗を返す。
- `convex_diameter_squared(polygon)`: 凸多角形の直径二乗を回転キャリパーで返す。
- `point_set_diameter_squared(points)`: 点集合を凸包化して直径二乗を返す。

## API別の時間計算量・空間計算量

- `closest_pair_squared(points)`: 時間 O(n log n)、追加領域 O(1)。
- `convex_diameter_squared(polygon)`: 時間 O(n)、追加領域 O(1)。
- `point_set_diameter_squared(points)`: 時間 O(n log n)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。 `convex_diameter_squared` の入力は境界順の凸多角形とする。
