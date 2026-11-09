---
title: Sum Shortest Path Query Costs (最短路クエリコスト総和) [ROHAAN]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/all_pairs/sum_shortest_path_query_costs.hpp
---

静的な重み付き有向グラフについて、指定された頂点対の最短距離の総和を求める。行列向け、非負疎グラフ向け、負辺を許す疎グラフ向けの API を使い分けられる。

## `ShortestPathCostQuery`

```cpp
struct ShortestPathCostQuery {
    int from;
    int to;
};
```

総和に加える最短路の始点と終点を表す。

## `ShortestPathCostEdge`

```cpp
template <class T>
struct ShortestPathCostEdge {
    int from;
    int to;
    T cost;
};
```

疎グラフの有向辺を表す。`from` から `to` へ重み `cost` の辺を張る。

## `sum_shortest_path_query_costs`

```cpp
template <class T>
T sum_shortest_path_query_costs(
    std::vector<std::vector<T>> distance,
    const std::vector<ShortestPathCostQuery>& queries,
    const T& inf
);
```

`distance[i][j]` を有向辺 $i \to j$ の重みとし、辺がない要素には `inf` を格納する。Floyd–Warshall 法で全点対最短路を求め、各クエリの最短距離を 1 回ずつ総和に加えて返す。負閉路が存在しても、どのクエリの最短路にも影響しなければ使える。

### 時間計算量

頂点数を $V$、クエリの個数を $Q$ とする。負閉路がなければ $O(V^3 + Q)$、負閉路がある場合は $O(V^3 + VQ)$。

### 空間計算量

$O(V^2)$

## `sum_nonnegative_shortest_path_query_costs`

```cpp
template <class T>
T sum_nonnegative_shortest_path_query_costs(
    int vertex_count,
    const std::vector<ShortestPathCostEdge<T>>& edges,
    const std::vector<ShortestPathCostQuery>& queries,
    const T& inf = std::numeric_limits<T>::max() / 4
);
```

全辺の重みが非負の疎グラフ向けの API である。クエリを始点ごとにまとめ、クエリに現れる始点からだけ Dijkstra 法を実行して距離を使い回す。

### 時間計算量

辺数を $E$、クエリに現れる相異なる始点の個数を $s$ として $O(V + E + s(V + E)\log V + Q)$。

### 空間計算量

$O(V + E + Q)$

## `sum_shortest_path_query_costs_johnson`

```cpp
template <class T>
T sum_shortest_path_query_costs_johnson(
    int vertex_count,
    const std::vector<ShortestPathCostEdge<T>>& edges,
    const std::vector<ShortestPathCostQuery>& queries,
    const T& inf = std::numeric_limits<T>::max() / 4
);
```

負の辺を許す疎グラフ向けの API である。Bellman–Ford 法でポテンシャルを求めて全辺の重みを非負に付け替え、クエリに現れる始点からだけ Dijkstra 法を実行する。

### 時間計算量

$O(VE + s(V + E)\log V + Q)$

### 空間計算量

$O(V + E + Q)$

## 注意点

- `distance` は正方行列でなければならない。重みがちょうど `inf` の直接辺は表現できない。
- 頂点数は非負で、辺の端点とクエリの端点は $[0, V)$ の範囲内でなければならない。違反時は `runtime_error` を送出する。
- `sum_nonnegative_shortest_path_query_costs` は負の辺を受け付けない（`runtime_error` を送出する）。
- `sum_shortest_path_query_costs_johnson` はグラフのどこにも負閉路が存在してはならない。
- 到達不能なクエリ、行列版の API でクエリに影響する負閉路、Johnson 法で検出した負閉路がある場合は `runtime_error` を送出する。
- 距離の計算結果と総和は `T` で表現できなければならない。
- `T` は加減算、比較、`T(0)` を提供する数値型でなければならない。浮動小数点型では丸め誤差により Johnson 法の付け替え後の重みがわずかに負になり `runtime_error` となる場合があるため、整数型を推奨する。
