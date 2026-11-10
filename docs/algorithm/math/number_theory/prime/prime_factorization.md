---
title: Prime Factorization (素因数分解) [FACT0] [factorize] [NTL_1_A] [NTL_1_D] [primality_test]
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/prime_factorization.hpp
---

64 bit 整数向けの Miller–Rabin 素数判定と Pollard の rho 法による素因数分解。

## 関数

- `math::is_prime_miller_rabin(n)`
    - 64 bit 整数に対する決定的な Miller–Rabin 判定。
- `math::is_prime_miller_rabin_random(n, rounds, rng)`
    - 乱数生成器 `rng` から底を選ぶ Miller–Rabin 判定。
- `math::pollard_rho(n)`
    - $n$ の非自明な約数を 1 つ返す。$n$ が素数なら $n$ を返す。
- `math::factorize_pollard_rho(n)`
    - $n$ を素因数分解して `(素数, 指数)` の列を返す。
- `math::euler_phi(n)`
    - オイラーのトーシェント関数 $\varphi(n)$ を返す。

## 時間計算量・空間計算量

以下で「期待」は Pollard の rho 法の反復回数に関する期待計算量である。$K$ は相異なる素因数の個数とする。

| API | 時間計算量 |
| --- | --- |
| `math::mul_mod_u64(a, b, mod)` | $O(1)$ |
| `math::pow_mod_u64(x, n, mod)` | $O(\log(n+1))$ |
| `math::miller_rabin_test(n, a, d, s)` | $O(\log n)$ |
| `math::is_prime_miller_rabin(n)` | $O(\log n)$（64 bit で固定個数の底） |
| `math::is_prime_miller_rabin_random(n, rounds, rng)` | $O(\mathrm{rounds} \cdot \log n)$ |
| `math::pollard_rho(n)` | 期待 $O(n^{1/4})$ |
| `math::factorize_pollard_rho_rec(n, factors)` | 期待 $O(n^{1/4} \log n)$、出力の追記数に依存 |
| `math::factorize_pollard_rho(n)` | 期待 $O(n^{1/4} \log n)$、出力 $O(K)$ |
| `math::euler_phi(n)` | 素因数分解と同じ期待計算量 |

`pollard_rho` は失敗すると再試行するので、反復回数に決定的な上界はない。`is_prime_miller_rabin_random` の誤判定確率は `rounds` に依存する。

## 注意点

- `u64`、`u128` は内部計算にも使う 64 bit / 128 bit 符号なし整数の別名である。
- `mul_mod_u64(a, b, mod)` は $ab \bmod \mathrm{mod}$、`pow_mod_u64(x, n, mod)` は $x^n \bmod \mathrm{mod}$ を返す。`mod > 0` が必要だが、実装は $0$ を検査しない。
- `miller_rabin_test(n, a, d, s)` は、$n-1 = d \cdot 2^s$（$d$ は奇数）と分解済みの 1 つの底 `a` に対する判定である。低水準の API であり、この前提は検査しない。
- `is_prime_miller_rabin(n)` はすべての 64 bit 整数で決定的な結果を返す。
- `is_prime_miller_rabin_random(n, rounds, rng)` は `rng` から底を選ぶ。`rounds < 0` では `std::runtime_error` を送出する。合成数に `true` を返す確率がある。
- `pollard_rho(n)` は $n > 1$ を要求し（違反時は `std::runtime_error`）、素数なら $n$、合成数なら非自明な約数を返す。乱数のシードは $n$ から決まるので結果は決定的である。
- `factorize_pollard_rho_rec(n, factors)` は素因数を重複込みで `factors` の末尾に追加する低水準の API で、$n \ge 1$ が必要である。
- `factorize_pollard_rho(n)` は素数と指数の組を素数の昇順で返す。$n = 1$ では空の列を返し、$n = 0$ では `std::runtime_error` を送出する。
- `euler_phi(n)` は $1 \le x \le n$ かつ $\gcd(x, n) = 1$ を満たす整数の個数を返す。`euler_phi(1) = 1` で、$0$ では `std::runtime_error` を送出する。
