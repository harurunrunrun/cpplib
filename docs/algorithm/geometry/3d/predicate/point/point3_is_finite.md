---
title: Point3 Finite Check (三次元点の有限値判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/point/point3_is_finite.hpp
---

## API

- `geometry3d_is_finite(point)` は `Point3` の全座標が有限なら `true` を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

座標値の有限性だけを判定し、点の用途に固有な退化条件は検査しない。
