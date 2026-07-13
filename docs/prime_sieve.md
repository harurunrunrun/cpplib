---
title: Prime Sieve (素数篩)
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
- 篩本体は $O(n)$、固定長配列の初期化を含む `build` 全体は $O(MAX_N+n)$
- `is_prime`, `min_factor`, `prime`: $O(1)$
- `factorize`: $O(\log n)$

## API別計算量

$P=\pi(n)$、素因数の種類数を $K$ とする。

| API | 時間計算量 |
| --- | --- |
| `math::is_prime_sqrt(n)` | $O(\sqrt n)$ |
| `PrimeSieve(n)` | $O(MAX_N)$（`build` を含む） |
| `build(n)` | $O(MAX_N+n)$ |
| `size()` / `prime_count()` / `prime(k)` | $O(1)$ |
| `min_factor(n)` / `is_prime(n)` | $O(1)$ |
| `primes()` | $O(P)$（出力サイズ依存） |
| `factorize(n)` | $O(\log n+K)$（出力サイズ $K$） |

`build` は線形篩部分が $O(n)$ だが、固定長 `min_factor_data` の初期化に $O(MAX_N)$ かかる。保持領域は $O(MAX_N)$。
## APIの範囲と例外

- `is_prime_sqrt(n)` は $n\le1$ で `false`。例外は送出しない。
- constructorと `build(n)` は $0\le n\le MAX_N$ を要求する。
  `size()` は判定可能な最大値 `n` を返す。
- `prime_count()` は $[2,n]$ の素数数、`prime(k)` は0-indexedの
  $k$ 番目の素数。`k` が範囲外なら例外を送出する。
- `primes()` は構築範囲の素数を昇順でコピーして返す。
- `min_factor(x)` は $0\le x\le n$ を要求し、0では0、1では1を返す。
- `is_prime(x)` は $0\le x\le n$ を要求する。
- `factorize(x)` は $1\le x\le n$ を要求し、素因数と指数を昇順で返す。
- `min_factor_data`, `prime_data`, `_n`, `_prime_count` は公開storage。
  通常は対応するaccessorを使用する。
