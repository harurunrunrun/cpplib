---
title: Directed Spanning Arborescence Count (有向全域有向木数) [DAGCNT]
documentation_of: ../src/algorithm/graph/count_directed_spanning_arborescences.hpp
---

# count_directed_spanning_arborescences

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
| `count_directed_spanning_arborescences` | $O(N^4)$ | $O(N^2)$ |

## 注意点

- 隣接行列は空でない正方行列で、各要素は `'0'` または `'1'` でなければならない。
- 自己ループは数えない。
- 行列式は符号付き128 bitのBareiss法で厳密に計算する。
- 答えが `long long` を越える場合は `overflow_error`、入力前提に違反する場合は `runtime_error` を送出する。
