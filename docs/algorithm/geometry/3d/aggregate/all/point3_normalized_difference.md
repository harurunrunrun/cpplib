---
title: Normalized Point3 Difference (正規化三次元点差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/point3_normalized_difference.hpp
---

## API

- `Geometry3DNormalizedDifference` は正規化差 `value` と倍率 `scale` を保持する。`geometry3d_normalized_difference(left, right, additional_extents)` は両点と追加長さから安全な共通倍率を選ぶ。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

点と追加長さは有限、追加長さは非負でなければならない。違反時は `std::invalid_argument`、非零差を表現できない場合は `std::overflow_error` を送出する。
