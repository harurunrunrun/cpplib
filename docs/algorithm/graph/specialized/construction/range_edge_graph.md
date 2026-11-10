---
title: Range Edge Graph (区間辺グラフ)
documentation_of: ../../../../../src/algorithm/graph/specialized/construction/range_edge_graph.hpp
---

頂点番号の区間を端点とする有向辺を、セグメント木の補助頂点で圧縮して保持する重み付きグラフ。元の頂点は $[0, N)$ であり、区間はすべて半開区間 $[l, r)$ で指定する。

## テンプレート引数

```cpp
RangeEdgeGraph<Weight>
```

辺の重みの型を `Weight` とする。補助辺には、コンストラクタに渡した零重みを使う。

## 辺型

```cpp
struct Edge {
    int to;
    Weight weight;
};
using graph_type = std::vector<std::vector<Edge>>;
```

`to` は遷移先の内部頂点番号、`weight` は辺の重みである。元の頂点の番号は常に $[0, N)$ のまま保たれる。

## 構築

```cpp
explicit RangeEdgeGraph(int original_size, Weight zero = Weight())
```

`original_size` 個の元の頂点と、2 本のセグメント木に対応する補助頂点・零重みの辺を構築する。`zero` は、利用する最短路などの演算における加法単位元でなければならない。

## 頂点数・辺数

```cpp
int original_size() const
int vertex_count() const
size_t edge_count() const
```

それぞれ元の頂点数、補助頂点を含む内部の頂点数、現在の有向辺の本数を返す。

## 1 頂点から 1 頂点への辺

```cpp
void add_edge(int from, int to, const Weight& weight)
```

元の頂点 `from` から元の頂点 `to` へ、重み `weight` の辺を追加する。

## 1 頂点から区間への辺

```cpp
void add_point_to_range(
    int from,
    int left,
    int right,
    const Weight& weight
)
```

`from` から `[left, right)` の各頂点へ、重み `weight` の辺がある状態を追加する。空区間なら辺を追加しない。

## 区間から 1 頂点への辺

```cpp
void add_range_to_point(
    int left,
    int right,
    int to,
    const Weight& weight
)
```

`[left, right)` の各頂点から `to` へ、重み `weight` の辺がある状態を追加する。空区間なら辺を追加しない。

## 区間から区間への辺

```cpp
void add_range_to_range(
    int source_left,
    int source_right,
    int destination_left,
    int destination_right,
    const Weight& weight
)
```

`[source_left, source_right)` の各頂点から `[destination_left, destination_right)` の各頂点へ、重み `weight` で遷移できる状態を追加する。中継用の補助頂点（hub）を 1 個追加するため、各遷移で重み `weight` をちょうど 1 回だけ通る。どちらかが空区間なら何も追加しない。

## グラフの取得

```cpp
const graph_type& view() const
graph_type build() const
```

`view` は内部のグラフへの `const` 参照を返す。`build` は内部のグラフのコピーを返す。得られるグラフで最短路計算などを行う際は、元の頂点以外の補助頂点も探索の対象に含める。

## 時間計算量

$N$ を元の頂点数、$V, E$ を現在の内部の頂点数・辺数とし、重みのコピーを $O(1)$ とする。

- コンストラクタ: $O(N)$
- `original_size`, `vertex_count`, `edge_count`, `view`: $O(1)$
- `add_edge`: 償却 $O(1)$
- `add_point_to_range`, `add_range_to_point`: $O(\log N)$
- `add_range_to_range`: $O(\log N)$
- `build`: $O(V + E)$

## 空間計算量

構築直後の頂点・辺は $O(N)$ である。追加した 1 対 1 の辺の本数を $Q_1$、その他の区間の辺の本数を $Q_2$ とすると、全体は $O(N + Q_1 + Q_2 \log N)$ である。

## 注意点

- `original_size <= 0` は `std::invalid_argument`。
- 区間で `left > right` の場合は `std::invalid_argument`。
- 元の頂点が $[0, N)$ の範囲外、または区間の端点が $[0, N]$ の範囲外の場合は `std::out_of_range`。
- 内部の頂点数が `int` で表現できない場合（`add_range_to_range` での補助頂点の追加を含む）は `std::length_error`。
- `view` が返す参照と、その内部要素への参照・イテレータは、次の追加操作で無効になりうる。
