---
title: Smallest Prime Factor Prefix Sum (最小素因数の累積和) [APS]
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/smallest_prime_factor_prefix_sum.hpp
---

$2$ から $n$ までの各整数の最小素因数の和を求めるための前計算表。

## `SmallestPrimeFactorPrefixSum`

```cpp
explicit math::SmallestPrimeFactorPrefixSum(int limit);
```

`limit` までの最小素因数をエラトステネスの篩で求め、その累積和を構築する。

## `limit`

```cpp
int limit() const;
```

表の上限を返す。

## `value`

```cpp
std::uint64_t value(int n) const;
```

$\sum_{i=2}^{n} \mathrm{spf}(i)$（$\mathrm{spf}(i)$ は $i$ の最小素因数）を返す。$n \le 1$ では $0$ を返す。

## 時間計算量・空間計算量

$L$ を `limit` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(L \log \log L)$ | $O(L)$ |
| `limit` | $O(1)$ | $O(1)$ |
| `value` | $O(1)$ | $O(1)$ |

## 注意点

- `limit` は非負でなければならない。負の場合は `std::invalid_argument` を送出する意図の検査があるが、表の確保が先に行われるので、実際には確保の時点で失敗する。
- $0 \le n \le L$ の範囲外のクエリでは `std::out_of_range` を送出する。
