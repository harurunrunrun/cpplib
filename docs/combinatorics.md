---
title: Combinatorics
documentation_of: ../src/algorithm/math/combinatorics.hpp
---

階乗、組合せ、順列、Bell 数。

`Mint` は `inv()` を持つ modint 型を想定する。

## 使い方

```cpp
using mint = Modint<998244353>;
Combinatorics<mint, 200000> comb;
mint x = comb.combination(n, k);
```

## メンバ関数

- `Combinatorics()`
    - `MAX_N` までの階乗と逆階乗を構築する。
- `Mint factorial(n) const`
    - `n!` を返す。
- `Mint inverse_factorial(n) const`
    - `(n!)^{-1}` を返す。
- `Mint inverse(n) const`
    - `n^{-1}` を返す。
- `Mint permutation(n, k) const`
    - ${}_nP_k$ を返す。
- `Mint combination(n, k) const`
    - ${}_nC_k$ を返す。
- `Mint homogeneous(n, k) const`
    - 重複組合せ ${}_{n+k-1}C_k$ を返す。
- `Mint bell_number(n) const`
    - Bell 数を返す。

## 計算量

- 構築: $O(MAX_N)$
- `factorial`, `inverse_factorial`, `inverse`, `permutation`, `combination`, `homogeneous`: $O(1)$
- `bell_number`: $O(n^2)$
