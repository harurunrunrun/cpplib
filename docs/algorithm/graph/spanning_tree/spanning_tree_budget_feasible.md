---
title: Spanning Tree Budget Feasibility (全域木予算充足判定)
documentation_of: ../../../../src/algorithm/graph/spanning_tree/spanning_tree_budget_feasible.hpp
---

無向重み付きグラフに、辺の重みの総和が予算以下となる全域木が存在するかを判定する。

## API

### `spanning_tree_budget_feasible`

```cpp
bool spanning_tree_budget_feasible(
    int vertex_count,
    const std::vector<KruskalEdge<long long>>& edges,
    long long budget
);
```

Kruskal 法で最小全域木を求め、全域木が存在してその重みが `budget` 以下なら `true` を返す。グラフが非連結なら `false` を返す。負の辺の重みと負の予算も扱える。

## 計算量

$N$ を `vertex_count`、$M$ を `edges` の要素数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `spanning_tree_budget_feasible` | $O(N + M\log(M + 1))$ | $O(N + M)$ |

## 注意点

- `vertex_count` は非負で、各辺の端点は $[0, N)$ の範囲内でなければならない。頂点数が負なら `invalid_argument`、端点が範囲外なら `out_of_range` を送出する。
- 頂点数 0 のグラフでは、空の全域木を重み $0$ として扱う。
- 最小全域木の重みは符号付き 128 ビット整数で計算してから予算と比較する。
