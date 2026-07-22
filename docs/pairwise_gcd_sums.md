---
title: Pairwise GCD Sums (ペアごとの最大公約数総和) [GCDEX]
documentation_of: ../src/algorithm/math/number_theory/pairwise_gcd_sums.hpp
---

複数の上限について、異なる正整数ペアの最大公約数の総和を一括計算する。

## API

```cpp
std::vector<std::uint64_t> pairwise_gcd_sums(
    const std::vector<int>& upper_bounds
);
```

`upper_bounds[i] = n` ごとに

$$
\sum_{1 \leq left < right \leq n} \gcd(left, right)
$$

を入力順に返す。`n = 0, 1` の総和は $0$ であり、空の入力には空の配列を返す。

## API別の時間計算量・空間計算量

`N = max(upper_bounds)`, `Q = upper_bounds.size()` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `pairwise_gcd_sums` | $O(N\log N + Q)$ | $O(N)$ |

## 注意点

- `upper_bounds` の要素が負なら `std::invalid_argument` を送出する。
- 正整数だけをペアの候補とし、同じ整数同士および順序を入れ替えただけのペアは数えない。
- 戻り値の各要素は `std::uint64_t` に収まる必要がある。
