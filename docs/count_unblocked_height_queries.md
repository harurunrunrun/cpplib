---
title: Count Unblocked Height Queries (高さに遮られない問い合わせ数) [THRBL]
documentation_of: ../src/algorithm/range/count_unblocked_height_queries.hpp
---

高さ列上の有向な2点問い合わせのうち、始点より高い内部要素が存在しない問い合わせ数を返す。

## count_unblocked_height_queries

```cpp
template<int MAX_SIZE = 50000>
int count_unblocked_height_queries(
    const vector<int>& height,
    const vector<pair<int, int>>& queries
);
```

問い合わせ `(from, to)` ごとに、両端を除く区間の最大値が `height[from]` 以下なら成功として数える。隣接点と同一点の問い合わせは常に成功とする。

## 時間計算量

列長を $N$、問い合わせ数を $Q$ として、構築 $O(N\log N)$、問い合わせ全体 $O(Q)$。

## 空間計算量

$O(N\log N)$。

## 注意点

- 全頂点番号は $[0,N)$ でなければならない。
- $N$ は `MAX_SIZE` 以下でなければならない。
- 前提違反時は `runtime_error` を送出する。
