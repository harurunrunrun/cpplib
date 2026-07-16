---
title: Associative Sieve Summatory Quotients (結合的篩による商ごとの総和) [ASSIEVE]
documentation_of: ../src/algorithm/math/associative_sieve.hpp
---

4状態の可換モノイド上に値を持つ乗法的関数について、`floor(n / i)` ごとの冪和をまとめて求める。

## `AssociativeSieveOperation`

```cpp
using AssociativeSieveOperation = array<array<uint8_t, 4>, 4>;
```

状態の二項演算表を表す。状態0が単位元であり、演算は可換かつ結合的でなければならない。

## `AssociativeSieveValue`

```cpp
using AssociativeSieveValue = array<Modint998244353, 4>;
```

添字 `r` の要素が状態 `r` に属する整数の冪和を表す。

## `associative_sieve_summatory_quotients`

```cpp
vector<AssociativeSieveValue> associative_sieve_summatory_quotients(
    uint64_t n,
    uint32_t exponent,
    const AssociativeSieveOperation& operation
);
```

素数冪に対する状態を `(prime * power_exponent) % 4` とし、互いに素な積の状態を `operation` で合成する。返り値の `i - 1` 番目は、`1 <= value <= floor(n / i)` に対する `value ^ exponent` の状態別総和である。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `associative_sieve_summatory_quotients` | $\widetilde O(n^{3/4} + \sqrt n + exponent^2)$ | $O(\sqrt n\log n + exponent)$ |

## 注意点

- `1 <= n <= 10^10`、`exponent <= 1000` が必要である。違反時は `std::invalid_argument` を送出する。
- 演算表の値域、単位元、可換性、結合性を検査し、不正なら `std::invalid_argument` を送出する。
- 結果は `998244353` を法とする。
