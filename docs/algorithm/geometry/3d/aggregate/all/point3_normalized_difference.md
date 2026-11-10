---
title: Normalized Point3 Difference (正規化三次元点差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/point3_normalized_difference.hpp
---

## API

- `Geometry3DNormalizedDifference`: 正規化した差 `value`（`Point3`）と倍率 `scale`（`long double`）を保持する。
- `geometry3d_normalized_difference(left, right, additional_extents = {})`: 2 点の差 `left - right` を、オーバーフローせずに表現できる共通の倍率 `scale` で割った値を返す。`additional_extents`（`std::initializer_list<long double>`）で与えた追加の長さも、同じ倍率で安全に扱えるように倍率を選ぶ。

## API 別の時間計算量・空間計算量

- 時間計算量: $O(1)$（`additional_extents` の個数を定数とみなす）。
- 追加空間計算量: $O(1)$。

## 注意点

点と追加の長さは有限で、追加の長さは非負でなければならない。違反時は `std::invalid_argument` を送出する。非零の差を表現できない場合は `std::overflow_error` を送出する。
