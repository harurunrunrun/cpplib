---
title: Smallest Prime Factor Prefix Sum (最小素因数の累積和) [APS]
documentation_of: ../src/algorithm/math/smallest_prime_factor_prefix_sum.hpp
---

`2..n` の各整数の最小素因数を合計する前計算表。

## `SmallestPrimeFactorPrefixSum`

```cpp
explicit SmallestPrimeFactorPrefixSum(int limit);
```

上限までの最小素因数と累積和を構築する。

## `limit`

```cpp
int limit() const;
```

表の上限を返す。

## `value`

```cpp
uint64_t value(int n) const;
```

`sum(spf(i), 2 <= i <= n)` を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(limit\log\log limit)$ | $O(limit)$ |
| `limit` | $O(1)$ | $O(1)$ |
| `value` | $O(1)$ | $O(1)$ |

## 注意点

- 範囲外クエリでは `out_of_range` を送出する。
