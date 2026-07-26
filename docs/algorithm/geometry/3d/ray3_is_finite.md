---
title: Ray3 Finite Check (三次元半直線の有限値判定)
documentation_of: ../../../../src/algorithm/geometry/3d/ray3_is_finite.hpp
---

## API

- `geometry3d_is_finite(ray)` は `Ray3` の始点と通過点の全座標が有限なら `true` を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

始点と通過点の一致は検査しない。
