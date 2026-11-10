---
title: Ray3 Finite Check (三次元半直線の有限値判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray/ray3_is_finite.hpp
---

## API

- `geometry3d_is_finite(ray)`: `Ray3` の始点 `origin` と通過点 `through` の全座標が有限なら `true` を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

- 例外は送出しない（`noexcept`）。
- 始点と通過点の一致（退化した半直線）は検査しない。
