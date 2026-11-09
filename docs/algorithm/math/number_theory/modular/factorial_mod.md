---
title: Factorial Mod (階乗の剰余)
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/factorial_mod.hpp
---

素数 $p$ を法とする階乗を求める。$n! = a \cdot p^e$（$p \nmid a$）と表したときの $a \bmod p$ と $e$ を返す。

## 使い方

```cpp
math::FactorialModPrime fac(1000000007);
auto res = fac.factorial(n);  // res.value = a mod p, res.exponent = e
```

## メンバ関数

- `explicit FactorialModPrime(int prime)`
    - $0!, 1!, \ldots, (p-1)!$ を前計算する。`prime < 2` の場合は `std::runtime_error` を送出する。
- `int mod() const`
    - 法を返す。
- `FactorialModPrimeResult factorial(long long n) const`
    - $n!$ から $p$ の因子をすべて取り除いた値を $p$ で割った余り `value` と、$p$ の指数 `exponent` を返す（いずれも `long long`）。`n < 0` の場合は `std::runtime_error` を送出する。

## 時間計算量・空間計算量

| API | 時間計算量 | 補足 |
| --- | --- | --- |
| `FactorialModPrime(prime)` | $O(p)$ | 保持する領域も $O(p)$。 |
| `mod()` | $O(1)$ | |
| `factorial(n)` | $O(\log_p(n+1))$ | 戻り値の `value` / `exponent` の参照は各 $O(1)$。 |

## 注意点

- `prime` が素数であることは検査しない（Wilson の定理を使うので、素数でないと結果は正しくない）。
- 前計算表を `std::vector<int>` で持つので、$p$ が大きいとメモリを $O(p)$ 消費する。
