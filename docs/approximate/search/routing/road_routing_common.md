---
title: Road Routing Common Utilities (道路網探索共通補助)
documentation_of: ../../../../src/approximate/search/routing/road_routing_common.hpp
---

道路網向け前処理・経路照会の各実装が共有する内部補助である。通常は
`arc_flags.hpp` などの公開ヘッダを直接 include し、このヘッダの
`road_routing_internal` を利用者コードから呼ばない。

## `Graph<Cost>`

```cpp
template<class Cost>
using Graph = vector<vector<pair<int, Cost>>>;
```

頂点番号と非負辺コストからなる隣接リストである。型の別名付け自体は
時間・空間計算量 $O(1)$。

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

`infinity`、`add`、`validate_vertex`、`optional_distance` は時間計算量
$O(1)$。頂点数を $V$、辺数を $E$ とすると、`validate_graph` と
`reverse_graph` は時間計算量 $O(V+E)$、空間計算量はそれぞれ
$O(1)$ と $O(V+E)$。`dijkstra` は二分ヒープを用い、時間計算量
$O((V+E)\log V)$、空間計算量 $O(V+E)$。`all_pairs_distances` は
時間計算量 $O(V(V+E)\log V)$、空間計算量 $O(V^2+V+E)$。

## 注意点

内部実装用APIであり、互換性を保証する公開インターフェースではない。
頂点番号は $[0,V)$、辺コストは有限かつ非負でなければならない。
`infinity<Cost>()` と同じ値は有限辺コストとして使用できない。`add` は
overflowまたは到達不能を `infinity<Cost>()` に飽和させる。
