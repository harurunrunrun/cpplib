---
title: Maximum Paths with Distinct Terminal Edges (終端辺を共有しない最大パス数) [POTHOLE]
documentation_of: ../src/algorithm/graph/maximum_paths_with_distinct_terminal_edges.hpp
---

有向グラフで、始点から出る辺と終点へ入る辺だけを互いに共有しないように選べる
始点・終点間パスの最大本数を求めます。中間辺は共有できます。

## API

### `maximum_paths_with_distinct_terminal_edges(vertex_count, directed_edges, source, sink)`

```cpp
long long maximum_paths_with_distinct_terminal_edges(
    int vertex_count,
    const vector<pair<int, int>>& directed_edges,
    int source,
    int sink
);
```

各 `source` 出辺と各 `sink` 入辺の使用回数を1以下にし、それ以外の辺には
十分大きい容量を与えた最大流を返します。同じ向きの多重辺は別々の終端辺として
扱います。

この関数は通常の辺素パス数とは異なります。中間辺の重複は禁止しません。

## API別の時間計算量・空間計算量

$N=vertex\_count$、$M=directed\_edges.size()$ とします。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_paths_with_distinct_terminal_edges` | $O(N^2M)$ | $O(N+M)$ |

## 注意点

- `vertex_count` は非負、辺端点と両終端は $[0,N)$ に入る必要があります。
- `source != sink` が必要です。
- 自己loopは結果に影響しないため無視します。
- 前提違反には `invalid_argument`、`out_of_range` または
  `length_error` を送出します。
