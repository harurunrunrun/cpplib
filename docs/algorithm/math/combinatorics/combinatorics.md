---
title: Combinatorics (組合せ論)
documentation_of: ../../../../src/algorithm/math/combinatorics/combinatorics.hpp
---

階乗・逆階乗・逆元の表を前計算し、順列・組合せ・重複組合せ・Bell 数を求める。

`Mint` は `inv()` を持つ modint 型を想定する。

テンプレート引数 `MAX_N` を指定すると `std::array` を使う固定長版、省略すると実行時に上限を指定する `std::vector` 版になる。以下では表の上限（固定長版の `MAX_N`、可変長版のコンストラクタ引数 `maximum`）を $N$ と書く。

## 使い方

```cpp
using mint = Modint<998244353>;

Combinatorics<mint, 200000> comb;   // 固定長版
mint x = comb.combination(n, k);

Combinatorics<mint> dynamic(n);     // 可変長版（上限を実行時に指定）
```

## メンバ関数

- `Combinatorics()`（固定長版）/ `explicit Combinatorics(int maximum = 0)`（可変長版）
    - $N$ までの階乗と逆階乗、$1, \ldots, N$ の逆元を構築する。可変長版で `maximum < 0` なら `invalid_argument` を送出する。
- `Mint factorial(int n) const`
    - $n!$ を返す。
- `Mint inverse_factorial(int n) const`
    - $(n!)^{-1}$ を返す。
- `Mint inverse(int n) const`
    - $n^{-1}$ を返す。
- `Mint permutation(int n, int k) const`
    - ${}_nP_k$ を返す。
- `Mint combination(int n, int k) const`
    - ${}_nC_k$ を返す。
- `Mint homogeneous(int n, int k) const`
    - 重複組合せ $\binom{n+k-1}{k}$ を返す。
- `Mint bell_number(int n) const`
    - Bell 数 $B_n$（要素数 $n$ の集合の分割の総数）を返す。

公開メンバ `fact`, `inv_fact`, `inv` はそれぞれ $n!$、$(n!)^{-1}$、$n^{-1}$ の前計算表で、直接参照してもよい。`inv[0]` は $0$ である。

## 時間計算量

`Mint` の四則演算と `inv()` を $O(1)$ とした場合。

| API | 時間計算量 |
| --- | --- |
| コンストラクタ | $O(N)$ |
| `factorial(n)` / `inverse_factorial(n)` / `inverse(n)` | $O(1)$ |
| `permutation(n, k)` / `combination(n, k)` / `homogeneous(n, k)` | $O(1)$ |
| `bell_number(n)` | $O(n \log n)$ |
| 公開配列 `fact` / `inv_fact` / `inv` の 1 要素参照 | $O(1)$ |

保持領域は $O(N)$。`bell_number` の一時領域は $O(n)$。

`bell_number(n)` は $B_n = \sum_{i=0}^{n} \frac{i^n}{i!} \sum_{j=0}^{n-i} \frac{(-1)^j}{j!}$ を前計算済みの `inv_fact` と交代和の接頭辞和で評価する。$i^n$ は二分累乗で求める。

## 注意点

- コンストラクタは `fact[N].inv()` を呼ぶため、$N!$ が `Mint` 上で逆元を持つ必要がある。
- `factorial(n)`, `inverse_factorial(n)` は $0 \le n \le N$、`inverse(n)` は $1 \le n \le N$ を要求し、範囲外では `runtime_error` を送出する。
- `permutation(n, k)`, `combination(n, k)` は $n$ が $[0, N]$ の範囲外なら `runtime_error` を送出し、$k < 0$ または $k > n$ なら $0$ を返す。
- `homogeneous(n, k)` は $n, k \ge 0$ を要求する。`homogeneous(0, 0)` は $1$、$k > 0$ のとき `homogeneous(0, k)` は $0$ を返す。$n > 0$ のときは $n + k - 1 \le N$ が必要で、満たさなければ `runtime_error` を送出する。
- `bell_number(n)` は $0 \le n \le N$ を要求する。
