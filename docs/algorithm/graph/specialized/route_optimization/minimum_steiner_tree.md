---
title: Minimum Steiner Tree with Edge Reconstruction (辺復元付き最小シュタイナー木) [minimum_steiner_tree]
documentation_of: ../../../../../src/algorithm/graph/specialized/route_optimization/minimum_steiner_tree.hpp
---

非負重みの無向グラフで、指定した端点をすべて連結する最小シュタイナー木を求め、選んだ辺の ID も復元する。

## API

```cpp
template<class T>
MinimumSteinerTreeResult<T> minimum_steiner_tree(
    const vector<vector<MinimumSteinerTreeEdge<T>>>& graph,
    const vector<int>& terminals,
    T infinity = numeric_limits<T>::max() / 4
)
```

`graph[v]` は `MinimumSteinerTreeEdge<T>{to, cost, id}` の列である。
無向辺は両方向に、同じ非負の `id` と `cost` で追加する。
異なる辺には異なる ID を割り当てる。

戻り値 `MinimumSteinerTreeResult<T>{cost, edge_ids, exists}` の `exists` は、全端点を連結できるかを表す。
連結できる場合、`cost` は最小重み、`edge_ids` は選んだ辺の ID を昇順に並べた列である。
端点が空なら重み $0$ と空の辺集合を返す。
連結できない場合は `cost == infinity` で、`edge_ids` は空である。

## アルゴリズム

`dp[mask][v]` を、`mask` に含まれる端点をすべて頂点 `v` と連結する最小重みとする。
各 `mask` について、2 つの部分集合の併合を行った後、多始点 Dijkstra 法を行う。
各状態の更新元を保存しておき、最終状態から部分集合の併合と最短路を逆にたどって辺 ID を復元する。

## API別の時間計算量・空間計算量

頂点数を $N$、隣接リストの有向辺の総数を $M$、端点数を $K$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `minimum_steiner_tree` | $O(3^K N+2^K(N+M)\log(M+N+1))$ | $O(2^K N+M)$ |

## 注意点

- 端点の頂点番号や辺の行き先が範囲外、辺重みが負、辺 ID が負、または $K$ が `int` のビット数（`numeric_limits<int>::digits`）以上の場合は `runtime_error` を送出する。
- 到達可能性は `infinity` とは別に管理する。
- 加算結果は `T` で表現できる必要がある。
