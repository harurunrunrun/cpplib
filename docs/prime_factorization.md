---
title: Prime Factorization (素因数分解) [FACT0] [factorize] [NTL_1_A] [NTL_1_D] [primality_test]
documentation_of: ../src/algorithm/math/number_theory/prime_factorization.hpp
---

64 bit 整数向けの Miller-Rabin と Pollard's rho。

## 関数

- `math::is_prime_miller_rabin(n)`
    - 64 bit 整数に対する決定的 Miller-Rabin。
- `math::is_prime_miller_rabin_random(n, rounds, rng)`
    - `rng` から底を選ぶ Miller-Rabin。
- `math::pollard_rho(n)`
    - `n` の非自明な約数を 1 つ返す。`n` が素数なら `n` を返す。
- `math::factorize_pollard_rho(n)`
    - `n` を素因数分解して `(素数, 指数)` の列を返す。
- `math::euler_phi(n)`
    - Euler の $\varphi(n)$ を返す。

## 計算量の概要

- `is_prime_miller_rabin`: $O(\log n)$
- `pollard_rho`, `factorize_pollard_rho`: 期待 $O(n^{1/4})$
- `euler_phi`: 素因数分解に依存

## API別の時間計算量・空間計算量

以下で「期待」は Pollard's rho の反復回数に関する期待計算量である。

| API | 時間計算量 |
| --- | --- |
| `math::mul_mod_u64(a,b,mod)` | $O(1)$ |
| `math::pow_mod_u64(x,n,mod)` | $O(\log(n+1))$ |
| `math::miller_rabin_test(n,a,d,s)` | $O(\log n)$ |
| `math::is_prime_miller_rabin(n)` | $O(\log n)$（64 bitで固定個数の底） |
| `math::is_prime_miller_rabin_random(n,rounds,rng)` | $O(rounds\log n)$ |
| `math::pollard_rho(n)` | 期待 $O(n^{1/4})$ |
| `math::factorize_pollard_rho_rec(n,factors)` | 期待 $O(n^{1/4}\log n)$、出力追記数依存 |
| `math::factorize_pollard_rho(n)` | 期待 $O(n^{1/4}\log n)$、出力 $O(K)$ |
| `math::euler_phi(n)` | 素因数分解と同じ期待計算量 |

`pollard_rho` は再試行を行うため決定的な反復回数上界を置かない。`is_prime_miller_rabin_random` の誤判定確率は `rounds` に依存する。

## 注意点

- `u64`, `u128` は内部計算にも使う64/128 bit符号なし整数alias。
- `mul_mod_u64(a,b,mod)` は $ab\bmod mod$、`pow_mod_u64(x,n,mod)` は
  $x^n\bmod mod$ を返す。`mod > 0` が必要で、実装は0を検査しない。
- `miller_rabin_test(n,a,d,s)` は $n-1=d2^s$（$d$ は奇数）に分解済みの
  1つの底 `a` に対するtest。低水準APIであり、この前提を検査しない。
- `is_prime_miller_rabin(n)` は全64 bit整数で決定的な結果を返す。
- `is_prime_miller_rabin_random(n,rounds,rng)` は `rng` から底を選ぶ。
  `rounds < 0` は例外。合成数に `true` を返す確率がある。
- `pollard_rho(n)` は $n>1$ を要求し、素数なら `n`、合成数なら
  非自明な約数を返す。
- `factorize_pollard_rho_rec(n,factors)` は素因数を重複込みで末尾へ追加する
  低水準API。`n >= 1` が必要。
- `factorize_pollard_rho(n)` は素数と指数を昇順で返す。`n==1` は空、
  `n==0` は例外。
- `euler_phi(n)` は $1\le x\le n$ かつ $\gcd(x,n)=1$ の整数数を返す。
  `euler_phi(1)=1`、0では例外。
