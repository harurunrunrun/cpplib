---
title: Spanning Tree Budget Feasibility (全域木予算充足判定)
documentation_of: ../src/algorithm/graph/spanning_tree/spanning_tree_budget_feasible.hpp
---

無向重み付きグラフに、辺重みの総和が予算以下となる全域木が存在するかを判定します。

## API

### `spanning_tree_budget_feasible(vertex_count, edges, budget)`

```cpp
bool spanning_tree_budget_feasible(
    int vertex_count,
    const vector<KruskalEdge<long long>>& edges,
    long long budget
);
```

最小全域木が存在し、その重みが `budget` 以下なら `true` を返します。
グラフが非連結なら `false` を返します。負の辺重みと負の予算も扱えます。

## API別の時間計算量・空間計算量

$N=vertex\_count$、$M=edges.size()$ とします。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `spanning_tree_budget_feasible` | $O(M\log(M+1))$ | $O(N+M)$ |

## 注意点

- `vertex_count` は非負、各辺の端点は $[0,N)$ でなければなりません。
- 頂点数0のグラフでは空の全域木を重み0として扱います。
- 最小全域木の重みは符号付き128 bitで計算してから予算と比較します。
- 頂点数が負なら `invalid_argument`、端点が範囲外なら
  `out_of_range` を送出します。
