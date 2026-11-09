---
title: Long Double Finite Check (long double有限値判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/scalar/long_double_is_finite.hpp
---

## API

- `geometry3d_is_finite(value)`: `long double` の値 `value` が有限なら `true` を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

- NaN と正負の無限大には `false` を返す。例外は送出しない（`noexcept`）。
