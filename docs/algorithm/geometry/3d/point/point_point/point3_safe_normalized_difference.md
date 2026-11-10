---
title: Safe Normalized Point3 Difference (三次元点差の安全な正規化)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/point_point/point3_safe_normalized_difference.hpp
---

## API

- `geometry3d_safe_normalized_difference(left, right, positive_scale)`: 各座標について、差 `left - right` を `positive_scale` で割った値を、差の計算がオーバーフローしないように求めた `Point3` を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 各座標にスカラー版 `geometry3d_safe_normalized_difference(long double, long double, long double)` を適用する。前提と例外もスカラー版と同じである。
  - 座標または `positive_scale` が非有限、もしくは `positive_scale` が正でない場合は `std::invalid_argument` を送出する。
  - 差が非零なのに結果が $0$ にアンダーフローした場合は `std::overflow_error` を送出する。
