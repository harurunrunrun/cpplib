---
title: Sum Shortest Path Query Costs (最短路クエリコスト総和) [ROHAAN]
documentation_of: ../src/algorithm/graph/sum_shortest_path_query_costs.hpp
---

静的な重み付き有向グラフについて、指定された頂点対の最短距離の総和を求める。行列向け、非負疎グラフ向け、負辺を許す疎グラフ向けの API を使い分けられる。

## ShortestPathCostQuery

```cpp
struct ShortestPathCostQuery {
    int from;
    int to;
};
```

総和に加える最短路の始点と終点を表す。

## ShortestPathCostEdge

```cpp
template<class T>
struct ShortestPathCostEdge {
    int from;
    int to;
    T cost;
};
```

疎グラフの有向辺を表す。`from` から `to` へ重み `cost` の辺を張る。

## `sum_shortest_path_query_costs`

```cpp
template<class T>
T sum_shortest_path_query_costs(
    vector<vector<T>> distance,
    const vector<ShortestPathCostQuery>& queries,
    const T& inf
);
```

`distance[i][j]` を有向辺 $i\to j$ の重みとし、辺がない要素には `inf` を格納する。Floyd--Warshall 法で全点対最短路を構築し、各 query の最短距離を1回ずつ総和へ加えて返す。負閉路が存在しても、どの query の最短路にも影響しなければ利用できる。

### 時間計算量

頂点数を $V$、query 数を $Q$ とする。負閉路がなければ $O(V^3+Q)$、負閉路がある場合は $O(V^3+VQ)$。

### 空間計算量

$O(V^2)$。

## `sum_nonnegative_shortest_path_query_costs`

```cpp
template<class T>
T sum_nonnegative_shortest_path_query_costs(
    int vertex_count,
    const vector<ShortestPathCostEdge<T>>& edges,
    const vector<ShortestPathCostQuery>& queries,
    const T& inf = numeric_limits<T>::max() / 4
);
```

全辺が非負の疎グラフ向け API。query を始点ごとにまとめ、query に現れる始点からだけ Dijkstra 法を実行して距離を再利用する。

### 時間計算量

辺数を $E$、query に現れる異なる始点数を $s$ として、
$O(V+E+s(V+E)\log V+Q)$。

### 空間計算量

$O(V+E+Q)$。

## `sum_shortest_path_query_costs_johnson`

```cpp
template<class T>
T sum_shortest_path_query_costs_johnson(
    int vertex_count,
    const vector<ShortestPathCostEdge<T>>& edges,
    const vector<ShortestPathCostQuery>& queries,
    const T& inf = numeric_limits<T>::max() / 4
);
```

負辺を許す疎グラフ向け API。Bellman--Ford 法でポテンシャルを求めて全辺を非負に変換し、query に現れる始点からだけ Dijkstra 法を実行する。

### 時間計算量

$O(VE+s(V+E)\log V+Q)$。

### 空間計算量

$O(V+E+Q)$。

## 注意点

- `distance` は正方行列でなければならない。値がちょうど `inf` の直接辺は表現できない。
- 頂点数は非負、辺端点と query 端点は $[0,V)$ でなければならない。
- `sum_nonnegative_shortest_path_query_costs` は負辺を受け付けない。
- `sum_shortest_path_query_costs_johnson` はグラフのどこにも負閉路が存在してはならない。
- 到達不能な query、行列 API で query に影響する負閉路、Johnson 法で検出した負閉路がある場合は `runtime_error` を送出する。
- 距離計算と総和の結果は `T` で表現できる必要がある。
- `T` は加減算、比較、`T(0)` を提供する数値型でなければならない。浮動小数点型では丸め誤差により Johnson 法の縮約重みがわずかに負になる場合があるため、整数型を推奨する。
