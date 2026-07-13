---
title: Combinatorics (組合せ論)
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

## 計算量の概要

- 構築: $O(MAX_N)$
- `factorial`, `inverse_factorial`, `inverse`, `permutation`, `combination`, `homogeneous`: $O(1)$
- `bell_number`: $O(n^2)$

## API別の時間計算量・空間計算量

`Mint` の四則演算と `inv()` を $O(1)$ とした場合。

| API | 時間計算量 |
| --- | --- |
| `Combinatorics()` | $O(MAX_N)$（前処理） |
| `factorial(n)` / `inverse_factorial(n)` / `inverse(n)` | $O(1)$ |
| `permutation(n,k)` / `combination(n,k)` / `homogeneous(n,k)` | $O(1)$ |
| `bell_number(n)` | $O(n^2)$ |
| 公開配列 `fact` / `inv_fact` / `inv` の1要素参照 | $O(1)$ |

保持領域は $O(MAX_N)$。`bell_number` の呼出しごとの一時領域は $O(MAX_N)$。
## 注意点

- `fact[n]`, `inv_fact[n]`, `inv[n]` はそれぞれ $n!$、$(n!)^{-1}$、
  $n^{-1}$ の公開前計算表。`inv[0]` は0。
- constructorは `fact[MAX_N].inv()` を呼ぶため、$MAX_N!$ が `Mint` 上で
  逆元を持つ必要がある。
- `factorial(n)`, `inverse_factorial(n)` は $0\le n\le MAX_N$、
  `inverse(n)` は $1\le n\le MAX_N$ を要求し、範囲外では例外を送出する。
- `permutation(n,k)`, `combination(n,k)` は $n$ の範囲外では例外、
  $k<0$ または $k>n$ では0を返す。
- `homogeneous(n,k)` は重複組合せを返す。`n,k` は非負で、
  $n+k-1\le MAX_N$ が必要。`homogeneous(0,0)=1`、`homogeneous(0,k)=0\ (k>0)`。
- `bell_number(n)` は要素数 $n$ の集合の分割数を返す。
  $0\le n\le MAX_N$ が必要。
