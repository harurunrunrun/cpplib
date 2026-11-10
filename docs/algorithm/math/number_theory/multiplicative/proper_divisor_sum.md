---
title: Proper Divisor Sum (真の約数の総和) [DIVSUM2]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/proper_divisor_sum.hpp
---

64 bit 整数を素因数分解し、自分自身を除く正の約数の総和を返す。

## `proper_divisor_sum`

```cpp
std::uint64_t math::proper_divisor_sum(std::uint64_t n);
```

$\sum_{d \mid n, \, d < n} d = \sigma(n) - n$ を返す。$n = 1$ の結果は $0$ である。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `proper_divisor_sum` | 素因数分解の時間（Pollard–Rho 法の期待計算量） | $O(\log n)$ |

## 注意点

- $n \ge 1$ が必要で、`n == 0` では `std::invalid_argument` を送出する。
- 結果が `uint64_t` に収まらない場合は `std::overflow_error` を送出する（$\sigma(n)$ は `unsigned __int128` で計算する）。
