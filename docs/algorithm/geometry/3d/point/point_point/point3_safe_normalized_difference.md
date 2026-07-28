---
title: Safe Normalized Point3 Difference (安全な正規化三次元点差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/point_point/point3_safe_normalized_difference.hpp
---

## API

- `geometry3d_safe_normalized_difference(left, right, positive_scale)` は各座標差を安全に正規化した `Point3` を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

スカラー版と同じ前提・例外を各座標に適用する。
