---
title: Count Undominated Rank Triples (非支配順位三つ組の個数) [NICEDAY]
documentation_of: ../src/algorithm/other/sequence/count_undominated_rank_triples.hpp
---

3種類の順位を持つ要素のうち、3順位すべてがより小さい別要素を持たない要素数を返す。

## count_undominated_rank_triples

```cpp
template<int MAX_SIZE = 100000>
int count_undominated_rank_triples(
    const vector<array<int, 3>>& ranks
);
```

順位は0-indexedで渡す。

## 時間計算量

要素数を $N$ として $O(N\log N)$。

## 空間計算量

$O(N)$。

## 注意点

- 各順位は $[0,N)$ でなければならない。
- 各軸の順位は相異なることを呼び出し側が保証する。
- $N$ は `MAX_SIZE` 以下でなければならない。
- 前提違反時は `runtime_error` を送出する。
