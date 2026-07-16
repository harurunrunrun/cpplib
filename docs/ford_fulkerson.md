---
title: Ford Fulkerson (フォード・ファルカーソン法)
documentation_of: ../src/algorithm/matching/ford_fulkerson.hpp
---

`FordFulkerson` という既存APIを保った最大流。
`max_flow` は流量値に依存する単純な増加路反復ではなく、
Dinic法のlevel graphとblocking flowを使う。

## コンストラクタ

```cpp
FordFulkerson<T> graph(n)
```

## 関数

```cpp
graph.add_edge(from, to, cap)
graph.max_flow(s, t, flow_limit)
```

`max_flow` は整数・実数容量のどちらでも、容量値そのものに依存しない
回数のlevel graph構築で残余流を追加する。

`flow_limit` は非負である必要がある。`s == t` の場合は `0` を返す。

self-loop を追加できるが、最大流量には寄与しない。

## 時間計算量

$V$ を頂点数、$E$ を追加済み辺数とする。

- `FordFulkerson(n)`: $O(V)$
- `add_edge`: 償却 $O(1)$
- `dfs`: 互換用の増加路探索1回につき $O(V+E)$
- `max_flow`: 一般容量で $O(EV^2)$

`max_flow` の上界は最大流量 $F$ や、容量の数値の大きさに依存しない。

## 空間計算量

- 残余グラフとDFS領域を含めて $O(V+E)$

## 注意点

`FordFulkersonEdge` は `to`、逆辺index `rev`、残余容量 `cap` を持つ。
互換性のためpublicの `graph`, `used`, `dfs` を維持する。
`dfs` は呼出し側が `used` を0で初期化してから使う低水準APIであり、
通常は `max_flow` を使う。`max_flow` 自体は `dfs` を使わない。
`level` と `iter` はDinic法の作業配列で、`max_flow` の途中状態だけを表す。
複数回の `max_flow` は現在の残余graphから続行し、
各呼出しで新たに流した量を返す。

頂点数は非負、各端点は対応する頂点範囲内でなければならず、違反時は `runtime_error`。容量・cost・総和の演算結果が `T` に収まることを前提とする。記載した計算量には引数検査とResultの構築を含む。
