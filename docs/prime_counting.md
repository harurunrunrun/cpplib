---
title: Prime Counting
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
