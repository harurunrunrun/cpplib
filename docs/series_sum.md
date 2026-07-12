---
title: Series Sum
documentation_of: ../src/algorithm/math/series_sum.hpp
---

等差数列、等比数列の和。

## 関数

- `math::arithmetic_progression_sum(first, diff, n)`
    - 初項 `first`、公差 `diff`、項数 `n` の等差数列の和を返す。
- `math::geometric_series_pow_sum(ratio, n)`
    - `{ratio^n, 1 + ratio + ... + ratio^{n-1}}` を返す。
- `math::geometric_series_sum(ratio, n)`
    - `1 + ratio + ... + ratio^{n-1}` を返す。
- `math::geometric_progression_sum(first, ratio, n)`
    - 初項 `first`、公比 `ratio`、項数 `n` の等比数列の和を返す。

## 計算量

- `arithmetic_progression_sum`: $O(1)$
- `geometric_series_pow_sum`, `geometric_series_sum`, `geometric_progression_sum`: $O(\log n)$
