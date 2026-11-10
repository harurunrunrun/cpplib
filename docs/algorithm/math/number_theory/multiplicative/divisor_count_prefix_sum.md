---
title: Divisor Count Prefix Sum (約数個数の累積和) [DIVCNT1]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/divisor_count_prefix_sum.hpp
---

正整数の約数の個数の累積和を、128 bit 整数で厳密に返す。

## `divisor_count_prefix_sum`

```cpp
math::divisor_count_prefix_sum_value math::divisor_count_prefix_sum(std::uint64_t n);
```

$\sum_{i=1}^{n} \tau(i)$（$\tau(i)$ は $i$ の正の約数の個数）を返す。`n` は累積和の右端で、`n = 0` では $0$ を返す。戻り値の型 `divisor_count_prefix_sum_value` は `unsigned __int128` の別名である。

双曲線 $xy = n$ の下の格子点を Stern–Brocot 木で凸包に沿って数える方法を用いる。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `divisor_count_prefix_sum` | $O(n^{1/3} \log n)$ | $O(n^{1/3})$ |

## 注意点

- $n < 2^{63}$ が必要で、範囲外では `std::invalid_argument` を送出する。
- 戻り値を標準出力に書き出すときは、128 bit 整数に対応した変換を用意する必要がある。
