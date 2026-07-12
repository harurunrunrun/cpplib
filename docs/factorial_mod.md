---
title: Factorial Mod
documentation_of: ../src/algorithm/math/factorial_mod.hpp
---

素数 `p` に対する階乗。

`n! = a p^e` としたとき、`a mod p` と `e` を返す。

## 使い方

```cpp
math::FactorialModPrime fac(1000000007);
auto res = fac.factorial(n);
```

## メンバ関数

- `FactorialModPrime(prime)`
    - `0!, 1!, ..., (prime - 1)!` を前計算する。
- `int mod() const`
    - 法を返す。
- `FactorialModPrimeResult factorial(n) const`
    - `n!` から `prime` の因子を取り除いた値と、`prime` の指数を返す。

## 計算量

- 構築: $O(p)$
- `factorial`: $O(\log_p n)$
