---
title: Minimum Rank-Removal Sort Cost (順位費用付き除去ソートの最小費用) [LSORT]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/sequence_selection/minimum_rank_removal_sort_cost.hpp
---

順列から要素を 1 個ずつ除去し、毎回残っている値の最小値または最大値を除去するときの、最小の総費用を区間 DP で求める。

## minimum_rank_removal_sort_cost

```cpp
long long minimum_rank_removal_sort_cost(
    const std::vector<int>& permutation
);
```

未除去の値が連続区間 $[l,r]$ をなす状態から、値 $l$ または $r$ を除去する。除去する直前の未除去の要素数を $k = r-l+1$、元の順列で除去する要素より左にある除去済みの要素の個数に $1$ を加えたものを $x$ とするとき、その操作の費用を $kx$ とする。すべての要素を除去するときの最小の総費用を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_rank_removal_sort_cost` | $O(N^2)$ | $O(N^2)$ |

## 注意点

- 入力は $1$ から $N$ までを各 1 回ずつ含む順列でなければならない。違反時は `std::runtime_error` を送出する。
- 空の順列には $0$ を返す。
- 長さが `int` の範囲（から $2$ を引いた値）を超える場合は `std::length_error`、結果が `long long` を超える場合は `std::overflow_error` を送出する。
