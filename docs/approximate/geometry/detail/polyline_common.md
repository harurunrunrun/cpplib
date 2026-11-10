---
title: Polyline Geometry Helpers (折れ線幾何補助)
documentation_of: ../../../../src/approximate/geometry/detail/polyline_common.hpp
---

折れ線の簡略化で共有する、検査・距離・面積の内部 API である。以下、次元を $D$ とする。

## API

### `validate_polyline_point(point)`

全ての座標が有限であることを検査する。時間計算量は $O(D)$、追加空間計算量は $O(1)$ である。

### `point_segment_squared_distance(point, first, second)`

点と閉線分の距離の二乗を返す。長さ 0 の退化した線分にも対応する。時間計算量は $O(D)$、追加空間計算量は $O(1)$ である。

### `doubled_triangle_area(first, middle, last)`

2 次元の 3 点が作る三角形の面積の 2 倍を返す。時間計算量・追加空間計算量はともに $O(1)$ である。

## 注意点

- `approximate::geometry::detail` の内部 API である。
- 非有限の座標は、事前に `validate_polyline_point` で拒否する（`std::invalid_argument`）。距離や面積が `long double` に収まらない場合は `std::overflow_error` を送出する。
