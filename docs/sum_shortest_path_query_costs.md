---
title: Sum Shortest Path Query Costs (最短路クエリコスト総和) [ROHAAN]
documentation_of: ../src/algorithm/graph/sum_shortest_path_query_costs.hpp
---

静的な重み付き有向グラフについて、指定された頂点対の最短距離の総和を求める。

## ShortestPathCostQuery

```cpp
struct ShortestPathCostQuery {
    int from;
    int to;
};
```

総和に加える最短路の始点と終点を表す。

## sum_shortest_path_query_costs

```cpp
template<class T>
T sum_shortest_path_query_costs(
    vector<vector<T>> distance,
    const vector<ShortestPathCostQuery>& queries,
    const T& inf
);
```

`distance[i][j]` を有向辺 $i\to j$ の重みとし、辺がない要素には `inf` を格納する。各queryの最短距離を1回ずつ総和へ加えて返す。

## 時間計算量

頂点数を $N$、query数を $Q$ とする。負閉路がなければ $O(N^3+Q)$、負閉路がある入力では $O(N^3+NQ)$。

## 空間計算量

$O(N^2)$。

## 注意点

- `distance` は正方行列でなければならない。
- query端点は $[0,N)$ でなければならない。
- 到達不能なquery、または到達可能な負閉路の影響で最短距離が定まらないqueryがある場合は `runtime_error` を送出する。
- 距離計算と総和の結果は `T` で表現できる必要がある。
- 値がちょうど `inf` の直接辺は表現できない。
