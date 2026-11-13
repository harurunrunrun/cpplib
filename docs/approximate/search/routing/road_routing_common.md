---
title: Road Routing Common Utilities (道路網探索共通補助)
documentation_of: ../../../../src/approximate/search/routing/road_routing_common.hpp
---

道路網向けの前処理・経路照会の各実装が共有する内部補助である。通常は `arc_flags.hpp` などの公開ヘッダを直接 include し、このヘッダの名前空間 `road_routing_internal` の関数を利用者コードから呼ばない。

## `Graph<Cost>`

```cpp
template<class Cost>
using Graph = std::vector<std::vector<std::pair<int, Cost>>>;
```

（行き先の頂点番号, 非負の辺コスト）の組からなる隣接リストである。型の別名なので時間・空間計算量は $O(1)$。

## 補助関数

```cpp
infinity<Cost>()
add(lhs, rhs)
validate_graph(graph)
validate_vertex(vertex, vertex_count)
reverse_graph(graph)
dijkstra(graph, source)
all_pairs_distances(graph)
optional_distance(distance)
```

- `infinity<Cost>()`: `std::numeric_limits<Cost>::max()` を返す。
- `add(lhs, rhs)`: 飽和加算。どちらかが `infinity<Cost>()` か、和がオーバーフローするなら `infinity<Cost>()` を返す。
- `validate_graph(graph)`: 行き先が範囲外、または辺コストが負・非有限・`infinity<Cost>()` に等しいなら `std::invalid_argument` を投げる。
- `validate_vertex(vertex, vertex_count)`: 頂点番号が $[0, \texttt{vertex_count})$ の範囲外なら `std::out_of_range` を投げる。
- `reverse_graph(graph)`: 全辺の向きを反転したグラフを返す。
- `dijkstra(graph, source)`: 二分ヒープによる単一始点最短距離。到達不能な頂点は `infinity<Cost>()`。
- `all_pairs_distances(graph)`: 全頂点を始点とした `dijkstra` の結果を並べた距離表。
- `optional_distance(distance)`: `infinity<Cost>()` なら `std::nullopt`、それ以外ならその値を返す。

## 時間計算量

頂点数を $V$、辺数を $E$ とする。

- `infinity`、`add`、`validate_vertex`、`optional_distance`: 時間計算量 $O(1)$
- `validate_graph`: 時間計算量 $O(V+E)$、追加空間 $O(1)$
- `reverse_graph`: 時間・空間計算量 $O(V+E)$
- `dijkstra`: 時間計算量 $O((V+E)\log V)$、空間計算量 $O(V+E)$
- `all_pairs_distances`: 時間計算量 $O(V(V+E)\log V)$、空間計算量 $O(V^2+V+E)$

## 注意点

- 内部実装用の API であり、互換性を保証する公開インターフェースではない。
- 頂点番号は $[0,V)$、辺コストは有限かつ非負でなければならない。
- `infinity<Cost>()` と同じ値は辺コストとして使用できない。
- `add` はオーバーフローまたは到達不能を `infinity<Cost>()` に飽和させる。
- `Cost` は `bool` 以外の算術型でなければならない（`static_assert`）。
