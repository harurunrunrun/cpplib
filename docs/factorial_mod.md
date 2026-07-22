---
title: Factorial Mod (階乗の剰余)
documentation_of: ../src/algorithm/math/number_theory/factorial_mod.hpp
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

## 計算量の概要

- 構築: $O(p)$
- `factorial`: $O(\log_p n)$

## API別の時間計算量・空間計算量

| API | 時間計算量 | 補足 |
| --- | --- | --- |
| `FactorialModPrime(prime)` | $O(p)$ | $p=prime$。前処理と保持領域はいずれも $O(p)$。 |
| `mod()` | $O(1)$ | 構築時の法を返す metadata query。 |
| `factorial(n)` | $O(\log_p(n+1))$ | 戻り値の `value` / `exponent` 参照は各 $O(1)$。 |

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
