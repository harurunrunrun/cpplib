---
title: Prime Factorization
documentation_of: ../src/algorithm/math/prime_factorization.hpp
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

## 計算量

- `is_prime_miller_rabin`: $O(\log n)$
- `pollard_rho`, `factorize_pollard_rho`: 期待 $O(n^{1/4})$
- `euler_phi`: 素因数分解に依存
