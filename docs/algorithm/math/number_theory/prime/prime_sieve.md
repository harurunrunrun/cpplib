---
title: Prime Sieve (素数篩)
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/prime_sieve.hpp
---

試し割りによる素数判定と、最小素因数を記録する線形篩。

## 関数

- `math::is_prime_sqrt(n)`
    - 試し割りで素数判定する。

## `PrimeSieve`

```cpp
math::PrimeSieve<1000000> fixed(n);  // 固定長版
math::PrimeSieve<> dynamic(n);       // 可変長版
```

`PrimeSieve<MAX_N>` は `MAX_N` を容量とする固定長の `std::array` に最小素因数と素数列を持つ。テンプレート引数を省略する（デフォルトの `-1`）と、実行時のサイズに合わせて `std::vector` を使う可変長版になり、再構築のたびに現在の上限に合わせて配列を取り直す。

## メンバ関数

- `PrimeSieve(int n = MAX_N)`（可変長版のデフォルトは `n = 0`）
    - $n$ まで篩を構築する。
- `void build(int n)`
    - $n$ まで作り直す。
- `int size() const`
    - 構築した上限 $n$（判定できる最大値）を返す。
- `int prime_count() const`
    - $[2, n]$ の素数の個数を返す。
- `int prime(int k) const`
    - 0-indexed で $k$ 番目の素数を返す。
- `std::vector<int> primes() const`
    - 構築範囲の素数を昇順に並べた列をコピーして返す。
- `int min_factor(int x) const`
    - $x$ の最小素因数を返す。$x = 0$ では $0$、$x = 1$ では $1$ を返す。
- `bool is_prime(int x) const`
    - $x$ が素数かを返す。
- `std::vector<std::pair<int, int>> factorize(int x) const`
    - $x$ を素因数分解して `(素数, 指数)` の列を素数の昇順に返す。

## 時間計算量・空間計算量

$P = \pi(n)$、素因数の種類数を $K$ とする。

| API | 時間計算量 |
| --- | --- |
| `math::is_prime_sqrt(n)` | $O(\sqrt n)$ |
| `PrimeSieve(n)` / `build(n)` | 固定長版 $O(\texttt{MAX_N} + n)$、可変長版 $O(n)$ |
| `size()` / `prime_count()` / `prime(k)` | $O(1)$ |
| `min_factor(x)` / `is_prime(x)` | $O(1)$ |
| `primes()` | $O(P)$（出力サイズに依存） |
| `factorize(x)` | $O(\log x)$（出力サイズ $K$ を含む） |

篩の本体は $O(n)$ だが、固定長版は `min_factor_data` 全体の初期化に $O(\texttt{MAX_N})$ かかる。保持領域は固定長版で $O(\texttt{MAX_N})$、可変長版で $O(n)$。

## 注意点

- `is_prime_sqrt(n)` は $n \le 1$ で `false` を返し、例外は送出しない。
- コンストラクタと `build(n)` は $0 \le n$（固定長版ではさらに $n \le \texttt{MAX_N}$）を要求する。
- `prime(k)` は $0 \le k <$ `prime_count()`、`min_factor(x)` と `is_prime(x)` は $0 \le x \le n$、`factorize(x)` は $1 \le x \le n$ を要求する。違反時はいずれも `std::runtime_error` を送出する。
- `min_factor_data`、`prime_data`、`_n`、`_prime_count` は公開されたメンバ変数だが、通常は対応するメンバ関数を使う。
- 固定長版は大きな `std::array` を 2 本メンバに持つので、スタックに置くとあふれる可能性がある。
