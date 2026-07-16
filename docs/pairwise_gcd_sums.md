---
title: Pairwise GCD Sums (ペアごとの最大公約数総和) [GCDEX]
documentation_of: ../src/algorithm/math/pairwise_gcd_sums.hpp
---

複数の上限について、異なる正整数ペアの GCD 総和を一括計算する。

## API

```cpp
vector<uint64_t> pairwise_gcd_sums(const vector<int>& upper_bounds);
```

各 `upper_bound = upper_bounds[i]` に対する
`sum(gcd(left, right), 1 <= left < right <= upper_bound)` を入力順に返す。
空の入力には空の配列を返す。

## API別の時間計算量・空間計算量

`N = max(upper_bounds)`, `Q = upper_bounds.size()` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `pairwise_gcd_sums` | $O(N\log N + Q)$ | $O(N)$ |

## 注意点

- `upper_bounds` の要素が負なら `std::invalid_argument` を送出する。
- 戻り値の各要素は `std::uint64_t` に収まる必要がある。
