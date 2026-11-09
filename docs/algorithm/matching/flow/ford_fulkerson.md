---
title: Ford Fulkerson (フォード・ファルカーソン法)
documentation_of: ../../../../src/algorithm/matching/flow/ford_fulkerson.hpp
---

`FordFulkerson` という既存の API を保ったまま最大流を求める。`max_flow` は流量の値に依存する単純な増加路の反復ではなく、Dinic 法のレベルグラフとブロッキングフローを用いる。

## コンストラクタ

```cpp
FordFulkerson<T> graph(n)
```

$n$ 頂点の空のネットワークを作る。

## メンバ関数

```cpp
graph.add_edge(from, to, cap)
graph.max_flow(s, t, flow_limit = numeric_limits<T>::max() / 4)
```

- `add_edge(from, to, cap)`: 容量 `cap` の有向辺を追加する。戻り値はない。
- `max_flow(s, t, flow_limit)`: `flow_limit` を上限として、`s` から `t` へ追加で流せる最大流量を返す。整数容量・実数容量のどちらでも、容量の値そのものに依存しない回数のレベルグラフ構築で残余流を追加する。

`flow_limit` は非負である必要がある。`s == t` の場合は `0` を返す。

自己ループを追加できるが、最大流量には寄与しない。

## 時間計算量

$V$ を頂点数、$E$ を追加済みの辺数とする。

- `FordFulkerson(n)`: $O(V)$
- `add_edge`: 償却 $O(1)$
- `dfs`: 互換用の増加路探索 1 回につき $O(V+E)$
- `max_flow`: 一般の容量で $O(V^2 E)$

`max_flow` の上界は、最大流量 $F$ や容量の数値の大きさに依存しない。

## 空間計算量

- 残余グラフと DFS の作業領域を含めて $O(V+E)$

## 注意点

`FordFulkersonEdge` は行き先 `to`、逆辺の添字 `rev`、残余容量 `cap` を持つ。互換性のため、公開メンバ `graph`, `used`, `dfs` を維持している。`dfs` は呼び出し側が `used` を $0$ で初期化してから使う低水準 API であり、通常は `max_flow` を使う。`max_flow` 自体は `dfs` を使わない。`level` と `iter` は Dinic 法の作業配列で、`max_flow` の途中状態だけを表す。`max_flow` を複数回呼ぶと現在の残余グラフから続行し、各呼び出しで新たに流した量を返す。

頂点数と容量は非負、各端点は頂点範囲内でなければならず、違反時は `runtime_error` を送出する。容量や流量の総和が `T` に収まることを前提とする。記載した計算量には引数検査を含む。
