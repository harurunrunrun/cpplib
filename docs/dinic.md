---
title: Dinic (Dinic法) [GRL_6_A]
documentation_of: ../src/algorithm/matching/dinic.hpp
---

Dinic法による最大流。

## コンストラクタ

```cpp
Dinic<T> graph(n)
```

## 関数

```cpp
graph.add_edge(from, to, cap)
graph.max_flow(s, t, flow_limit)
graph.min_cut(s)
```

`add_edge` は辺 id を返す。辺情報は `graph.edges[id]` で参照できる。

`flow_limit` は非負である必要がある。`s == t` の場合は `0` を返す。

self-loop を追加できるが、最大流量には寄与しない。

## 時間計算量

$V$ を頂点数、$E$ を追加済み辺数とする。

- `Dinic(n)`: $O(V)$
- `add_edge`: 償却 $O(1)$
- `bfs`: $O(V+E)$
- `dfs`: 1回 worst-case $O(V+E)$。1つのlevel graphでblocking flowを作る全呼出しは worst-case $O(VE)$
- `max_flow`: 一般容量で $O(V^2E)$
- `min_cut`: $O(V+E)$。長さ $V$ の到達列出力を含む

## 空間計算量

- 残余グラフ、level/iterator、戻り値を含めて $O(V+E)$

## 注意点

`DinicEdge` は `from`, `to`, 初期 `cap`, 現在の `flow` を持つ。public `edges` は正辺直後に逆辺を格納し、`add_edge` は正辺idを返す。public `bfs/dfs` はblocking-flow helperで、通常は `max_flow` を使う。`min_cut` は残余到達flag列を返す。

頂点数は非負、各端点は対応する頂点範囲内でなければならず、違反時は `runtime_error`。容量・cost・総和の演算結果が `T` に収まることを前提とする。記載した計算量には引数検査とResultの構築を含む。
