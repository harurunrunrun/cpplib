---
title: Maximum Paths with Distinct Terminal Edges (終端辺を共有しない最大パス数) [POTHOLE]
documentation_of: ../../../../src/algorithm/graph/flow_cut/maximum_paths_with_distinct_terminal_edges.hpp
---

有向グラフで、始点から出る辺と終点に入る辺だけは互いに共有しないように選べる、始点から終点へのパスの最大本数を求める。途中の辺は共有してよい。

## API

### `maximum_paths_with_distinct_terminal_edges`

```cpp
long long maximum_paths_with_distinct_terminal_edges(
    int vertex_count,
    const std::vector<std::pair<int, int>>& directed_edges,
    int source,
    int sink
);
```

`source` から出る各辺と `sink` に入る各辺の容量を $1$、それ以外の辺の容量を十分大きい値（辺数 $+1$）とした最大流を返す。同じ向きの多重辺は別々の終端辺として扱う。

通常の辺素パスの本数とは異なり、途中の辺の重複は禁止しない。

## 計算量

$N$ を `vertex_count`、$M$ を `directed_edges` の要素数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_paths_with_distinct_terminal_edges` | $O(N^2 M)$ | $O(N + M)$ |

## 注意点

- 自己ループは結果に影響しないため無視する。
- 前提条件の違反時には次の例外を送出する。
  - `vertex_count` が負、または `source == sink`: `invalid_argument`
  - `source`, `sink` または辺の端点が $[0, N)$ の範囲外: `out_of_range`
  - 辺数が `long long` の最大値以上: `length_error`
