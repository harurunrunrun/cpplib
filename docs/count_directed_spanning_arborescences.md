---
title: Directed Spanning Arborescence Count (有向全域有向木数) [DAGCNT]
documentation_of: ../src/algorithm/graph/count_directed_spanning_arborescences.hpp
---

## count_directed_spanning_arborescences

```cpp
long long count_directed_spanning_arborescences(
    const vector<string>& adjacency
);
```

隣接行列が表す有向グラフに含まれる、根から全辺が外向きの全域有向木の総数を返す。根が異なる有向木も別に数える。

## 引数

- `adjacency`: `'0'` と `'1'` からなる正方隣接行列。`adjacency[u][v] == '1'` は辺 $u\to v$ を表す。

## 戻り値

全域有向木の総数。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_directed_spanning_arborescences` | $O(N^3)$ | $O(N^2)$ |

## 注意点

- 頂点数は $1$ 以上 $8$ 以下で、隣接行列は正方、各要素は `'0'` または `'1'` でなければならない。
- 自己ループは数えない。
- 有向行列木定理と rank-one 更新
  $\det(L+\boldsymbol{1}\boldsymbol{1}^{\mathsf T})/N$ により、全ての根に対する余因子の和を1回の行列式計算で求める。
- 行列式は符号付き128 bitのBareiss法で厳密に計算する。
- 答えが `long long` を越える場合は `overflow_error`、入力前提に違反する場合は `runtime_error` を送出する。
