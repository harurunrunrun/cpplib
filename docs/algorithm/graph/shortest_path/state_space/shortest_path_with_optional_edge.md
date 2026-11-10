---
title: Shortest Path with Optional Edge (追加辺を高々1本使う最短路) [TRAFFICN]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/state_space/shortest_path_with_optional_edge.hpp
---

非負重みの有向グラフで、候補辺を高々 1 本だけ使えるときの 2 頂点間の最短距離を求める。始点からの Dijkstra 法と終点への（逆グラフ上の）Dijkstra 法を 1 回ずつ行い、各候補辺を試す。

## `OptionalShortestPathEdge<T>`

```cpp
template <class T>
struct OptionalShortestPathEdge { int from; int to; T cost; };
```

候補辺の始点・終点（0-indexed）と非負のコストを保持する。候補辺は元の `graph` には追加されず、選んだ高々 1 本だけを経路に使える。

## `shortest_path_with_at_most_one_optional_edge`

```cpp
template <class T>
std::optional<T> shortest_path_with_at_most_one_optional_edge(
    const std::vector<std::vector<DijkstraEdge<T>>>& graph,
    int source,
    int target,
    const std::vector<OptionalShortestPathEdge<T>>& optional_edges,
    bool optional_edges_are_bidirectional = false,
    T inf = std::numeric_limits<T>::max() / 4
);
```

`graph` は `DijkstraEdge<T>` で表した非負重みの有向グラフである。候補辺を使わない経路も含め、候補辺を高々 1 本使った `source` から `target` までの最短距離を返す。どの経路も存在しなければ `std::nullopt` を返す。

`optional_edges_are_bidirectional == false` なら、各候補辺を `from -> to` の有向辺として扱う。`true` なら、同じコストの `from -> to` と `to -> from` のどちらか一方向を高々 1 回使える。

`inf` の意味は `dijkstra` と同じである。

## 計算量

頂点数を $V$、既存の辺数を $E$、候補辺の個数を $K$ とする。

| API | 時間計算量 | 空間計算量（戻り値を除く） |
| --- | --- | --- |
| `shortest_path_with_at_most_one_optional_edge` | $O((V + E)\log(V + E + 1) + K)$ | $O(V + E)$ |

## 注意点

- `source`、`target`、辺の端点が範囲外の場合や、既存の辺・候補辺のコストが負の場合は `runtime_error` を送出する。
- `T` は距離の加算・比較ができ、すべての経路長を表現できる型でなければならない。
- 多重辺、自己ループ、空の候補集合を扱える。
