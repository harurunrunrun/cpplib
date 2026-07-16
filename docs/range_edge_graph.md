---
title: Range Edge Graph (区間辺グラフ) [range_edge_graph]
documentation_of: ../src/algorithm/graph/range_edge_graph.hpp
---

頂点番号の半開区間を端点とする有向辺を、segment treeの補助頂点で圧縮して保持する重み付きgraph。
元の頂点は $[0,N)$ であり、区間はすべて半開区間 $[l,r)$ で指定する。

## テンプレート引数

~~~cpp
RangeEdgeGraph<Weight>
~~~

辺重みの型を `Weight` とする。
補助辺にはconstructorへ渡した零重みを使用する。

## 辺型

~~~cpp
struct Edge {
    int to;
    Weight weight;
};
using graph_type = vector<vector<Edge>>;
~~~

`to` は遷移先の内部頂点番号、`weight` は辺重みである。
元の頂点番号は常に $[0,N)$ のまま保持される。

## 構築

~~~cpp
explicit RangeEdgeGraph(int original_size, Weight zero = Weight())
~~~

`original_size` 個の元頂点と、2本のsegment treeに対応する補助頂点・零重み辺を構築する。
`zero` は利用する最短路などの演算における加法単位元でなければならない。

## 頂点数・辺数

~~~cpp
int original_size() const
int vertex_count() const
size_t edge_count() const
~~~

元頂点数、補助頂点を含む内部頂点数、現在の有向辺数を返す。

## 1頂点から1頂点への辺

~~~cpp
void add_edge(int from, int to, const Weight& weight)
~~~

元頂点 `from` から元頂点 `to` へ重み `weight` の辺を追加する。

## 1頂点から区間への辺

~~~cpp
void add_point_to_range(
    int from,
    int left,
    int right,
    const Weight& weight
)
~~~

`from` から各 $v \in [left,right)$ へ、重み `weight` の辺がある状態を追加する。
空区間には辺を追加しない。

## 区間から1頂点への辺

~~~cpp
void add_range_to_point(
    int left,
    int right,
    int to,
    const Weight& weight
)
~~~

各 $u \in [left,right)$ から `to` へ、重み `weight` の辺がある状態を追加する。
空区間には辺を追加しない。

## 区間から区間への辺

~~~cpp
void add_range_to_range(
    int source_left,
    int source_right,
    int destination_left,
    int destination_right,
    const Weight& weight
)
~~~

各 $u \in [source_left,source_right)$ から各
$v \in [destination_left,destination_right)$ へ重み `weight` で遷移できる状態を追加する。
1個のhub補助頂点を追加するため、各遷移で重みをちょうど1回だけ通る。
どちらかが空区間なら何も追加しない。

## graphの取得

~~~cpp
const graph_type& view() const
graph_type build() const
~~~

`view` は内部graphへのconst参照を返す。
`build` は内部graphのcopyを返す。
得られるgraphでは元頂点以外も最短路計算などの探索対象に含める。

## 時間計算量

$N$ を元頂点数、$V,E$ を現在の内部頂点数・辺数とし、重みのcopyを $O(1)$ とする。

- constructor: $O(N)$
- `original_size`, `vertex_count`, `edge_count`, `view`: $O(1)$
- `add_edge`: $O(1)$ amortized
- `add_point_to_range`, `add_range_to_point`: $O(\log N)$
- `add_range_to_range`: $O(\log N)$
- `build`: $O(V+E)$

## 空間計算量

constructor直後の頂点・辺は $O(N)$。
追加した1対1辺を $Q_1$、その他の区間辺を $Q_2$ とすると、全体は
$O(N+Q_1+Q_2\log N)$。

## 注意点

- `original_size <= 0` は `std::invalid_argument`。
- 区間で `left > right` の場合は `std::invalid_argument`。
- 元頂点・区間端点が $[0,N]$ の許容範囲外の場合は `std::out_of_range`。
- 内部頂点数が `int` で表現できない場合は `std::length_error`。
- `view` が返す参照とその内部要素への参照・iteratorは、次の追加操作で無効になり得る。
