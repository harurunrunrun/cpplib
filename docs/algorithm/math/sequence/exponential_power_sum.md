---
title: Exponential Power Sums (冪と等比項の積の和) [sum_of_exponential_times_polynomial] [sum_of_exponential_times_polynomial_limit]
documentation_of: ../../../../src/algorithm/math/sequence/exponential_power_sum.hpp
---

素数 `MOD` を法として、等比項と冪の積の和を求める。$0^0 = 1$ とする。

## API

- `math::exponential_power_sum<MOD>(r, d, n)`: $\sum_{0 \le i < n} r^i i^d$ を返す。
- `math::exponential_power_sum_limit<MOD>(r, d)`: 有理関数としての無限和 $\sum_{i \ge 0} r^i i^d$ を返す。$r \ne 1$ が必要で、$r = 1$ なら `std::invalid_argument` を送出する。

$r = 0$、有限和での $r = 1$、$n = 0$ の場合も扱える。

## 時間計算量

線形篩で整数の $d$ 乗の表を作り、有限差分で無限和を求める。有限和は $r^n P(n) + C$（$P$ は $d$ 次多項式）の多項式部分を連続点から補間して求める。
時間 $O(d + \pi(d) \log (d+1))$ である（$\pi(d)$ は $d$ 以下の素数の個数）。最大次数が $10^7$ でも FFT 用の配列を必要としない。

## 空間計算量

$O(d)$。

## 使用例

区間 $[L, R)$ 上の重み付き冪和は、2 つの有限和の差で求められる。次数 $D$ の任意の多項式についての和は、各単項式の結果を係数で線形結合すればよい。

## 注意点

- `MOD` は $3$ 以上の素数でなければならない。
- $d + 2 < \mathrm{MOD}$ が必要であり、満たさない場合は `std::invalid_argument` を送出する（$r = 0$ の場合を除く）。
