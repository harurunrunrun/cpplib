---
title: Minimum Spanning Tree Total Weight (最小全域木の総重み) [MST]
documentation_of: ../../../../src/algorithm/graph/spanning_tree/minimum_spanning_tree_total_weight.hpp
---

重み付き無向グラフの最小全域木の総重みを Kruskal 法で求めて返す。

## `minimum_spanning_tree_total_weight`

```cpp
long long minimum_spanning_tree_total_weight(
    int vertex_count,
    const std::vector<std::tuple<int, int, long long>>& edges
);
```

`edges` の各要素を `(端点, 端点, 重み)` とする無向グラフについて、最小全域木の総重みを返す。

## 計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_spanning_tree_total_weight` | $O(N + M\log(M + 1))$ | $O(N + M)$ |

## 注意点

- `vertex_count` は正で、頂点番号は $0$ 以上 `vertex_count` 未満でなければならない。
- グラフは連結でなければならない。
- これらの前提に違反すると `runtime_error` を送出する。
- 総重みは `long long` に収まらなければならない（オーバーフローは検査しない）。
