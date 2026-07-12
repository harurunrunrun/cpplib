---
title: Segmented Sieve
documentation_of: ../src/algorithm/math/segmented_sieve.hpp
---

区間 `[l, r)` の素数判定をまとめて行う。

`0 <= l <= r` を仮定する。

## 関数

- `math::segmented_sieve(l, r)`
    - `l + i` が素数なら `true` となる列を返す。
- `math::primes_in_range(l, r)`
    - 区間 `[l, r)` に含まれる素数を昇順で返す。

## 計算量

- $O((r-l)\log\log r + \sqrt r \log\log r)$
