---
title: Basic Number Theory
documentation_of: ../src/algorithm/math/basic_number_theory.hpp
---

整数まわりの基本関数。

## 関数

- `math::to_base_digits(n, base)`
    - `n` を `base` 進数の桁列に変換する。上位桁から並ぶ。
- `math::from_base_digits(digits, base)`
    - `base` 進数の桁列を整数に戻す。
- `math::divisors(n)`
    - `n` の正の約数を昇順で返す。
- `math::quotient_ranges(n)`
    - `floor(n / i)` が等しい半開区間 `[l, r)` を列挙する。
- `math::ext_gcd(a, b)`
    - `a x + b y = gcd(a, b)` となる `{g, x, y}` を返す。
- `math::gcd(a, b)`
    - 最大公約数を返す。
- `math::lcm(a, b)`
    - 最小公倍数を返す。
- `math::isqrt(n)`
    - `floor(sqrt(n))` を返す。
- `math::pow_mod(x, n, mod)`
    - `x^n mod mod` を返す。
- `math::inv_mod(a, mod)`
    - `mod` が素数でなくてもよい場合の逆元を返す。
- `math::inv_mod_prime(a, mod)`
    - `mod` が素数の場合の逆元を返す。

## 計算量

- `to_base_digits`: $O(\log n)$
- `from_base_digits`: $O(|digits|)$
- `divisors`: $O(\sqrt n)$
- `quotient_ranges`: $O(\sqrt n)$
- `ext_gcd`, `gcd`, `lcm`, `inv_mod`: $O(\log \min(a, b))$
- `isqrt`: $O(1)$
- `pow_mod`, `inv_mod_prime`: $O(\log n)$

## API別計算量

$D$ を桁数、$K$ を返す要素数とする。固定幅整数の算術を $O(1)$ と数える。

| API | 時間計算量 |
| --- | --- |
| `math::abs_ll(x)` | $O(1)$ |
| `math::gcd(a,b)` / `ext_gcd(a,b)` / `lcm(a,b)` | $O(\log\max(|a|,|b|))$ |
| `math::pow_mod(x,n,mod)` | $O(\log(n+1))$ |
| `math::inv_mod(a,mod)` | $O(\log\max(|a|,mod))$ |
| `math::inv_mod_prime(a,mod)` | $O(\log mod)$ |
| `math::isqrt(n)` | $O(1)$ |
| `math::divisors(n)` | $O(\sqrt{|n|}+K)$、出力 $O(K)$ |
| `math::quotient_ranges(n)` | $O(\sqrt n)$、出力 $O(K)$（$K=O(\sqrt n)$） |
| `math::to_base_digits(n,base)` | $O(D)$、出力 $O(D)$ |
| `math::from_base_digits(digits,base)` | $O(D)$ |

`ExtGcdResult` と `QuotientRange` の各field参照は $O(1)$。
