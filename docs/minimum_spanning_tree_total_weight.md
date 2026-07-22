---
title: Minimum Spanning Tree Total Weight (最小全域木の総重み) [MST]
documentation_of: ../src/algorithm/graph/spanning_tree/minimum_spanning_tree_total_weight.hpp
---

重み付き無向グラフの最小全域木の総重みを返す。

## minimum_spanning_tree_total_weight

```cpp
long long minimum_spanning_tree_total_weight(
    int vertex_count,
    const vector<tuple<int, int, long long>>& edges
);
```

各 tuple を `(端点, 端点, 重み)` とする無向グラフの最小全域木の総重みを返す。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_spanning_tree_total_weight` | $O(M\log M)$ | $O(N+M)$ |

## 注意点

- `vertex_count` は正で、頂点番号は `0` 以上 `vertex_count` 未満でなければならない。
- グラフは連結でなければならない。
- 前提違反時は `runtime_error` を送出する。
- 総重みは `long long` に収まらなければならない。
