---
title: Count Undominated Rank Triples (非支配順位三つ組の個数) [NICEDAY]
documentation_of: ../../../../src/algorithm/other/sequence/count_undominated_rank_triples.hpp
---

3 種類の順位を持つ要素のうち、3 つの順位すべてが自分より小さい別の要素が存在しないものの個数を返す。第 1 順位で整列し、第 2 順位ごとの第 3 順位の最小値を区間最小のセグメント木で管理する。

## count_undominated_rank_triples

```cpp
template<int MAX_SIZE = -1>
int count_undominated_rank_triples(
    const vector<array<int, 3>>& ranks
);
```

順位は 0-indexed で渡す。

`MAX_SIZE` を省略すると、内部のセグメント木に実行時サイズの `std::vector` を使う可変長版になる。指定すると固定長版になる。

```cpp
auto result = count_undominated_rank_triples(ranks);
auto fixed = count_undominated_rank_triples<100000>(ranks);
```

## 時間計算量

要素数を $N$ として $O(N\log N)$。

## 空間計算量

可変長版は $O(N)$、固定長版は $O(N+\texttt{MAX_SIZE})$。

## 注意点

- 各順位は $[0,N)$ に属さなければならない。違反時は `runtime_error` を送出する。
- 各軸の順位が相異なることは呼び出し側が保証する（検査しない）。
- 固定長版では $N$ が `MAX_SIZE` 以下でなければならない。
