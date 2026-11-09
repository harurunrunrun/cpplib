---
title: Pairwise LCM Sum (全ペアLCM総和) [PRLCM]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/pairwise_lcm_sum.hpp
---

正整数列の相異なる添字のペアすべてについて、最小公倍数を合計する。

## `pairwise_lcm_sum`

```cpp
std::int64_t math::pairwise_lcm_sum(
    const std::vector<int>& values
);
```

`values` を $a_0, \ldots, a_{n-1}$ として、$\sum_{i < j} \operatorname{lcm}(a_i, a_j) \bmod 998244353$ を返す。

各 $d$ について $d$ の倍数である値どうしの積の和を求め、倍数側から包除して最大公約数がちょうど $d$ のペアの積の和に直し、$d$ で割って足し合わせる。

## 時間計算量・空間計算量

$M$ を `values` の最大値とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `pairwise_lcm_sum` | $O(M \log M + M \log 998244353)$ | $O(M)$ |

## 注意点

- 値はすべて正で、$998244353$ の倍数であってはならない。違反時は `std::invalid_argument` を送出する。
- 空間が $O(M)$ なので、値の最大値が大きいと大量のメモリを使う。
