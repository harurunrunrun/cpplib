---
title: Prime Counting (素数計数)
documentation_of: ../src/algorithm/math/prime_counting.hpp
---

素数の個数を数える。

# テンプレート引数

```cpp
PrimeCounting<MAX_N>
```

- 前計算する最大値 `MAX_N`

# コンストラクタ

```cpp
PrimeCounting(int n = MAX_N)
```

`0, ..., n` について前計算する。

## 時間計算量

- $O(N \log \log N)$

# query

```cpp
bool is_prime(int n)
int count_primes(int n)
int count_primes_less(int n)
```

`count_primes(n)` は `n` 以下の素数の個数を返す。

`count_primes_less(n)` は `n` 未満の素数の個数を返す。

## 時間計算量

- $O(1)$

# 大きな範囲の素数計数

```cpp
long long prime_count(long long n)
long long count_primes(long long n)
```

`n` 以下の素数の個数を返す。`count_primes` は `prime_count` の別名。
`floor(n / i)` の異なる値だけを保持し、エラトステネスの篩と同じ更新を行う。

## 計算量の概要

- 時間: $O(n^{3/4} / \log n)$
- 空間: $O(\sqrt n)$

`n < 0` の場合は `std::runtime_error` を送出する。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 補足 |
| --- | --- | --- |
| `math::prime_count(n)` | $O(n^{3/4}/\log n)$ | 保持領域 $O(\sqrt n)$。 |
| `math::count_primes(n)` | $O(n^{3/4}/\log n)$ | `prime_count` の別名。 |
| `PrimeCounting(n)` | $O(MAX_N+n\log\log n)$ | `build(n)` を含む前処理。 |
| `build(n)` | $O(MAX_N+n\log\log n)$ | 固定長配列の初期化を含む。 |
| `size()` | $O(1)$ | 構築済み上限を返す metadata query。 |
| `is_prime(n)` / `count_primes(n)` / `count_primes_less(n)` | $O(1)$ | 前処理後のquery。 |

`PrimeCounting` の保持領域は $O(MAX_N)$。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
