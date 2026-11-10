---
title: Pairwise GCD Sums (ペアごとの最大公約数総和) [GCDEX]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/pairwise_gcd_sums.hpp
---

複数の上限 $n$ について、相異なる正整数のペアの最大公約数の総和を一括で計算する。

## API

```cpp
std::vector<std::uint64_t> math::pairwise_gcd_sums(
    const std::vector<int>& upper_bounds
);
```

`upper_bounds` の各要素 $n$ に対して

$$
\sum_{1 \leq \mathrm{left} < \mathrm{right} \leq n} \gcd(\mathrm{left}, \mathrm{right})
$$

を入力順に返す。$n = 0, 1$ の総和は $0$ で、空の入力には空の配列を返す。

$\sum_{u < v} \gcd(u, v) = \sum_{d \mid v, \, d \ge 2} \varphi(d) \cdot (v/d)$ を、オイラー関数の篩と倍数への足し込みで全 $v \le N$ について求め、累積和をとる。

## 時間計算量・空間計算量

$N$ を `upper_bounds` の最大値、$Q$ を `upper_bounds` の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `pairwise_gcd_sums` | $O(N \log N + Q)$ | $O(N)$ |

## 注意点

- `upper_bounds` の要素が負なら `std::invalid_argument` を送出する。
- ペアの候補は正整数だけで、同じ整数同士のペアや、順序を入れ替えただけのペアは数えない。
- 戻り値の各要素は `std::uint64_t` に収まる必要がある。
