---
title: Count Unblocked Height Queries (高さに遮られない問い合わせ数) [THRBL]
documentation_of: ../../../../src/algorithm/range/offline_query/count_unblocked_height_queries.hpp
---

高さの列上の有向な 2 点の問い合わせのうち、両端の間に始点より高い要素が存在しないものの個数を返す。区間最大値をスパーステーブルで求める。

## count_unblocked_height_queries

```cpp
template<int MAX_SIZE = -1>
int count_unblocked_height_queries(
    const vector<int>& height,
    const vector<pair<int, int>>& queries
);
```

問い合わせ `(from, to)` ごとに、両端を除く区間の最大値が `height[from]` 以下なら成功として数える。隣接する 2 点と同一点の問い合わせは常に成功とする。

`MAX_SIZE` を省略すると、内部のスパーステーブルに実行時サイズの `std::vector` を使う可変長版になる。指定すると固定長版になる。

```cpp
auto count = count_unblocked_height_queries(heights, queries);
auto fixed = count_unblocked_height_queries<100000>(heights, queries);
```

## 時間計算量

列長を $N$、問い合わせ数を $Q$ として、構築に $O(N\log N)$、問い合わせ全体で $O(Q)$。

## 空間計算量

$O(N\log N)$。

## 注意点

- 問い合わせの頂点番号はすべて $[0,N)$ に属さなければならない。違反時は `runtime_error` を送出する。
- 固定長版では $N$ が `MAX_SIZE` 以下でなければならない。
