---
title: Polyline Geometry Helpers (折れ線幾何補助)
documentation_of: ../src/approximate/geometry/detail/polyline_common.hpp
---

## Polyline Geometry Helpers (折れ線幾何補助)

折れ線簡略化で共有する検査・距離・面積の内部API。

## API

### `validate_polyline_point(point)`

全座標が有限であることを検査する。次元を $D$ として時間計算量は $O(D)$、追加空間計算量は $O(1)$。

### `point_segment_squared_distance(point, first, second)`

点と閉線分の二乗距離を返す。退化線分にも対応する。時間計算量は $O(D)$、追加空間計算量は $O(1)$。

### `doubled_triangle_area(first, middle, last)`

2次元の3点が作る三角形の面積の2倍を返す。時間・追加空間計算量は $O(1)$。

## 注意点

`approximate::geometry::detail` の内部APIである。非有限座標は事前に `validate_polyline_point` で拒否する。距離または面積が `long double` に収まらない場合は `std::overflow_error` を送出する。
