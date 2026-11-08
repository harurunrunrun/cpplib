---
title: Normalized 3D Point Difference (三次元点差の正規化)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_point/point3_normalized_difference_points.hpp
---

2 点の差をオーバーフローさせずに、正規化した値と尺度の組で表す。

## API

- `geometry3d_normalized_difference(left, right, additional_extents = {})`: `left - right` を安全な尺度で正規化し、`Geometry3DNormalizedDifference{value, scale}` を返す。尺度は差の各成分の絶対値と `additional_extents`（`std::initializer_list<long double>`）の各要素の最大値で、差が零なら `{{0, 0, 0}, 1}` を返す。

## API別の時間計算量・空間計算量

- `geometry3d_normalized_difference`: `additional_extents` の要素数を $k$ として時間 $O(k)$、追加空間 $O(1)$。

## 注意点

- 戻り値 `result` は、表現可能な範囲で `result.value * result.scale == left - right` を満たす。
- 点または追加尺度が有限でない場合や、追加尺度が負の場合は `std::invalid_argument` を送出する。正規化後に非零成分が表現できない（アンダーフローして $0$ になる）場合は `std::overflow_error` を送出する。
