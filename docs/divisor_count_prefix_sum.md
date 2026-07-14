---
title: Divisor Count Prefix Sum (約数個数の累積和) [DIVCNT1]
documentation_of: ../src/algorithm/math/divisor_count_prefix_sum.hpp
---

正整数の約数個数の累積和を、128 bit 整数で厳密に返す。

# `divisor_count_prefix_sum`

```cpp
divisor_count_prefix_sum_value divisor_count_prefix_sum(uint64_t n);
```

`1 <= i <= n` に対する正の約数の個数の総和を返す。戻り値型
`divisor_count_prefix_sum_value` は `unsigned __int128` の別名である。`n = 0`
では `0` を返す。

## 引数

- `n`: 累積和の右端。

## 戻り値

`sum_{i=1}^n tau(i)`。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `divisor_count_prefix_sum` | $O(n^{1/3}\log n)$ | $O(n^{1/3})$ |

## 注意点

- `n < 2^63` を前提とする。範囲外では `std::invalid_argument` を送出する。
- 戻り値を標準出力へ書くときは、128 bit 整数に対応した変換を用意する。
