---
title: Normalized 3D Point Difference (三次元点差の正規化)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_point/point3_normalized_difference_points.hpp
---

## API

- `geometry3d_normalized_difference(left, right, additional_extents)`: `left - right` を安全な尺度で正規化し、正規化値と尺度を返す。

## API別の時間計算量・空間計算量

- `geometry3d_normalized_difference`: `additional_extents` の要素数を $k$ として時間 $O(k)$、追加領域 $O(1)$。

## 注意点

返却値 `result` は表現可能な範囲で `result.value * result.scale == left - right` を満たす。点または追加尺度が有限でない場合、追加尺度が負の場合は `std::invalid_argument`、非零成分を表現できない場合は `std::overflow_error` を送出する。
