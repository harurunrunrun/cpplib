---
title: Shortest Path with Optional Edge (追加辺を高々1本使う最短路) [TRAFFICN]
documentation_of: ../src/algorithm/graph/shortest_path_with_optional_edge.hpp
---

非負重み有向グラフで、候補辺を高々1本だけ使えるときの2頂点間最短距離を求める。
始点からのDijkstraと、辺を反転したグラフ上で終点から行うDijkstraを利用する。

## `OptionalShortestPathEdge<T>`

```cpp
OptionalShortestPathEdge<T>{from, to, cost}
```

候補辺の0-indexed始点・終点・非負コストを保持する。候補辺は元の`graph`へ追加されず、
選んだ高々1本だけを経路に使える。

## `shortest_path_with_at_most_one_optional_edge`

```cpp
std::optional<T> distance = shortest_path_with_at_most_one_optional_edge(
    graph, source, target, optional_edges,
    optional_edges_are_bidirectional, inf
);
```

`graph`は`DijkstraEdge<T>`で表す非負重み有向グラフ。候補を使わない経路も許し、
候補を高々1本使った`source`から`target`までの最短距離を返す。どの経路も存在
しなければ`std::nullopt`を返す。

`optional_edges_are_bidirectional == false`なら各候補を`from -> to`の有向辺として
扱う。`true`なら同じコストの`from -> to`と`to -> from`のどちらか一方向を、
高々1回使える。

`source`、`target`、辺の端点が範囲外、または既存辺・候補辺のコストが負なら
`std::runtime_error`を送出する。`inf`の意味は`dijkstra`と同じ。

## API別の時間計算量・空間計算量

頂点数を $V$、既存辺数を $E$、候補辺数を $K$ とする。

| API | 時間計算量 | 空間計算量（返り値を除く） |
| --- | --- | --- |
| `shortest_path_with_at_most_one_optional_edge` | $O((V+E)\log V+K)$ | $O(V+E)$ |

## 注意点

- `T`は距離の加算・比較ができ、全ての経路長を表現できる型でなければならない。
- 候補道路を建設した後に何度でも通れる問題でも、重みが正なら最短路は同じ道路を
  繰り返し使わないため、このAPIを適用できる。
- 平行辺、自己辺、空の候補集合を扱える。
