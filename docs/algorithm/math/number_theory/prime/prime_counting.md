---
title: Prime Counting (素数計数) [counting_primes]
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/prime_counting.hpp
---

素数の個数を数える。前計算表による $O(1)$ クエリの `PrimeCounting` と、大きな $n$ に対する素数計数関数 `prime_count` を提供する。

## `PrimeCounting`

### テンプレート引数

```cpp
math::PrimeCounting<MAX_N>   // 固定長版
math::PrimeCounting<>        // 可変長版（MAX_N = -1）
```

- `MAX_N`: 前計算する最大値（固定長の `std::array` の容量）。省略すると（デフォルトの `-1`）、実行時のサイズに合わせて `std::vector` を使う可変長版になる。

```cpp
math::PrimeCounting<> dynamic(n);
math::PrimeCounting<100000> fixed(n);
```

### コンストラクタ・`build`

```cpp
explicit PrimeCounting(int n = MAX_N)  // 可変長版のデフォルトは n = 0
void build(int n)
```

$0, \ldots, n$ についてエラトステネスの篩と素数の個数の累積和を前計算する。`n < 0`、または固定長版で `n > MAX_N` の場合は `std::runtime_error` を送出する。

### クエリ

```cpp
int size() const
bool is_prime(int n) const
int count_primes(int n) const
int count_primes_less(int n) const
```

- `size()`: 構築した上限 $n$ を返す。
- `is_prime(n)`: $n$ が素数かを返す。
- `count_primes(n)`: $n$ 以下の素数の個数を返す。
- `count_primes_less(n)`: $n$ 未満の素数の個数を返す。$n \le 0$ では $0$ を返す。

引数が前計算の範囲外の場合は `std::runtime_error` を送出する。

## 大きな範囲の素数計数

```cpp
long long math::prime_count(long long n)
long long math::count_primes(long long n)
```

$n$ 以下の素数の個数を返す。`count_primes` は `prime_count` の別名である。$\lfloor n/i \rfloor$ の相異なる値だけを保持し、エラトステネスの篩と同じ更新を行う。`n < 0` の場合は `std::runtime_error` を送出する。

## 時間計算量・空間計算量

| API | 時間計算量 | 補足 |
| --- | --- | --- |
| `math::prime_count(n)` | $O(n^{3/4}/\log n)$ | 保持領域 $O(\sqrt n)$。 |
| `math::count_primes(n)` | $O(n^{3/4}/\log n)$ | `prime_count` の別名。 |
| `PrimeCounting(n)` | 固定長版 $O(\texttt{MAX_N} + n \log \log n)$、可変長版 $O(n \log \log n)$ | `build(n)` を含む前処理。 |
| `build(n)` | 固定長版 $O(\texttt{MAX_N} + n \log \log n)$、可変長版 $O(n \log \log n)$ | 固定長版は配列全体の初期化を含む。 |
| `size()` | $O(1)$ | |
| `is_prime(n)` / `count_primes(n)` / `count_primes_less(n)` | $O(1)$ | 前処理の範囲内のクエリ。 |

`PrimeCounting` の保持領域は、固定長版で $O(\texttt{MAX_N})$、可変長版で $O(n)$。

## 注意点

- 固定長版は `std::array` を 2 本メンバに持つので、`MAX_N` が大きいとオブジェクトが大きくなる（スタックに置くとあふれる可能性がある）。
- 素数の個数は `int`（`PrimeCounting`）または `long long`（`prime_count`）で返す。
