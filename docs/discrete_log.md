---
title: Discrete Log
documentation_of: ../src/algorithm/math/discrete_log.hpp
---

離散対数問題。

`a^x = b (mod m)` となる最小の非負整数 `x` を返す。存在しない場合は `-1`。

## 関数

- `math::discrete_log_coprime(a, b, mod)`
    - `gcd(a, mod) = 1` の場合。
- `math::discrete_log(a, b, mod)`
    - `gcd(a, mod) != 1` の場合も処理する。

## 計算量

- $O(\sqrt {mod})$
