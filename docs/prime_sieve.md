---
title: Prime Sieve
documentation_of: ../src/algorithm/math/prime_sieve.hpp
---

素数判定と線形篩。

`PrimeSieve` は `MAX_N` までの最小素因数を持つ。

## 関数

- `math::is_prime_sqrt(n)`
    - 試し割りで素数判定する。

## PrimeSieve

```cpp
math::PrimeSieve<1000000> sieve(n);
```

## メンバ関数

- `PrimeSieve(int n = MAX_N)`
    - `n` まで篩を構築する。
- `void build(int n)`
    - `n` まで作り直す。
- `int size() const`
    - 構築済みの上限を返す。
- `int prime_count() const`
    - `n` 以下の素数の個数を返す。
- `int prime(int k) const`
    - `k` 番目の素数を返す。
- `vector<int> primes() const`
    - 素数列を返す。
- `int min_factor(int n) const`
    - `n` の最小素因数を返す。
- `bool is_prime(int n) const`
    - 素数かを返す。
- `vector<pair<int,int>> factorize(int n) const`
    - `n` を素因数分解して `(素数, 指数)` の列を返す。

## 計算量

- `is_prime_sqrt`: $O(\sqrt n)$
- 篩の構築: $O(n)$
- `is_prime`, `min_factor`, `prime`: $O(1)$
- `factorize`: $O(\log n)$
