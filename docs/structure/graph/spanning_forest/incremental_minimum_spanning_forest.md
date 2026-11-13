---
title: Incremental Minimum Spanning Forest (逐次最小全域森) [incremental_minimum_spanning_forest]
documentation_of: ../../../../src/structure/graph/spanning_forest/incremental_minimum_spanning_forest.hpp
---

空の重み付き無向グラフに辺を追加しながら、最小全域森を管理する。
森の辺を頂点として持つ Link-Cut Tree でパス上の最大重みの辺を取得し、閉路ができた場合は新しい辺より重い辺だけを置き換える。

## コンストラクタ

```cpp
IncrementalMinimumSpanningForest<Weight, MAX_VERTICES = -1, MAX_EDGES = -1> forest(int vertex_count);
```

頂点集合を $[0, \texttt{vertex_count})$ とする空の森を構築する。
`MAX_VERTICES` と `MAX_EDGES` は頂点数と追加する辺数の上限である。省略（`-1`）すると `std::vector` で頂点と追加済みの辺だけを保持し、辺を追加するたびに Link-Cut Tree のノードを増やす可変長版になる。

```cpp
IncrementalMinimumSpanningForest<long long> dynamic(vertex_count);
IncrementalMinimumSpanningForest<long long, 100000, 200000> fixed(vertex_count);
```

## add_edge

```cpp
int add_edge(int first, int second, const Weight& weight);
```

無向辺を追加する。辺番号は追加順に $0, 1, \dots$ と振られる。
新しい辺が森に入るだけなら `-1` を、閉路から取り除いた辺がある場合はその辺番号を返す。新しい辺自身が不要な場合（自己ループを含む）は、新しい辺の番号を返す。

## contains_edge

```cpp
bool contains_edge(int edge) const;
```

指定した辺が現在の最小全域森に含まれるかを返す。

## vertex_count / edge_count / component_count

```cpp
int vertex_count() const;
int edge_count() const;
int component_count() const;
```

頂点数、これまでに追加した辺数、現在の連結成分数を返す。

## 時間計算量

頂点数を $V$、追加した辺数を $E$ とする。

- コンストラクタ: 可変長版は $O(V)$、固定長版は $O(V + \texttt{MAX_EDGES})$
- `add_edge`: 償却 $O(\log(V+E))$
- `contains_edge`、`vertex_count`、`edge_count`、`component_count`: $O(1)$

## 空間計算量

可変長版は $O(V+E)$、固定長版は $O(V + \texttt{MAX_EDGES})$。各関数の追加領域は $O(1)$ である。

## 注意点

- `Weight` は全順序を持ち、`numeric_limits<Weight>::lowest()` が使える型でなければならない。
- 重みが相異なる場合、管理される最小全域森は一意である。同じ重みの辺どうしでは、既に森にある辺を優先する（新しい辺の重みが閉路上の最大重みと等しい場合は置き換えない）。
- 頂点数が負または上限を超える場合は `std::invalid_argument`、頂点や辺番号が範囲外の場合は `std::out_of_range`、辺数が容量を超える場合は `std::length_error` を送出する。
