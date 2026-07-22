---
title: Minimum Steiner Tree with Edge Reconstruction (辺復元付き最小シュタイナー木) [minimum_steiner_tree]
documentation_of: ../src/algorithm/graph/specialized/minimum_steiner_tree.hpp
---

無向非負重みグラフで、指定した端点をすべて連結する最小Steiner treeを求め、
選択した辺IDも復元する。

```cpp
MinimumSteinerTreeResult<T> minimum_steiner_tree(
    graph,
    terminals,
    infinity
)
```

`graph[v]` は `MinimumSteinerTreeEdge<T>{to, cost, id}` の列。
無向辺は両方向に同じ非負の `id` と `cost` で追加する。
異なる辺には異なるIDを割り当てる。

結果の `exists` は全端点を連結できるかを表す。
連結できる場合、`cost` は最小重み、`edge_ids` は選択した辺IDを昇順に並べた列。
端点が空なら重み0、空の辺集合を返す。
連結できない場合は `cost == infinity`、`edge_ids` は空。

## アルゴリズム

`dp[mask][v]` を、`mask` に含まれる端点を頂点 `v` で連結する最小重みとする。
部分集合二つの併合後、各 `mask` についてmulti-source Dijkstraを行う。
各更新元を保存し、最終状態から部分集合併合と最短路を逆にたどって辺IDを復元する。

## API別の時間計算量・空間計算量

頂点数を $N$、有向隣接辺数を $M$、端点数を $K$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `minimum_steiner_tree` | $O(3^K N+2^K M\log(N+1))$ | $O(2^K N+M)$ |

## 注意点

頂点番号、辺ID、辺の行き先が範囲外、辺重みが負、または `K` が
`int` のbit数以上なら `runtime_error` を送出する。
加算結果は `T` で表現できる必要がある。
