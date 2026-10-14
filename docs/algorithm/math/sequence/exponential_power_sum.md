---
title: Exponential Power Sums (冪と等比項の積の和) [sum_of_exponential_times_polynomial] [sum_of_exponential_times_polynomial_limit]
documentation_of: ../../../../src/algorithm/math/sequence/exponential_power_sum.hpp
---

素数法 `MOD` で `0^0=1` とする。

- `math::exponential_power_sum<MOD>(r,d,n)`: `sum_{0<=i<n} r^i*i^d`。
- `math::exponential_power_sum_limit<MOD>(r,d)`: 有理関数としての無限和。`r!=1` が必要。

## 時間計算量

`d+2<MOD` が必要。`r=0`、有限和の `r=1`、`n=0` を扱う。線形篩で整数の `d` 乗表を作り、有限差分で無限和を求める。有限和は `r^n*P(n)+C` の多項式部分を連続点補間する。時間 $O(d+\pi(d)\log(d+1))$、領域 $O(d)$。最大次数1000万でも FFT 配列を必要としない。

類題: 区間 `[L,R)` の重み付き冪和を、2つの有限和の差で求める。任意の次数Dの多項式なら各単項式の結果を係数で線形結合できる。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
