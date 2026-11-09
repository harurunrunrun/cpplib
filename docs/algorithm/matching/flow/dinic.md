---
title: Dinic (Dinic法) [GRL_6_A]
documentation_of: ../../../../src/algorithm/matching/flow/dinic.hpp
---

Dinic 法で最大流を求める。

## コンストラクタ

```cpp
Dinic<T> graph(n)
```

$n$ 頂点の空のネットワークを作る。

## メンバ関数

```cpp
graph.add_edge(from, to, cap)
graph.max_flow(s, t, flow_limit = numeric_limits<T>::max() / 4)
graph.min_cut(s)
```

- `add_edge(from, to, cap)`: 容量 `cap` の有向辺を追加し、辺番号を返す。辺の情報は `graph.edges[id]` で参照できる。
- `max_flow(s, t, flow_limit)`: `s` から `t` へ、`flow_limit` を上限として最大流を流し、その流量を返す。`flow_limit` は非負である必要がある。`s == t` の場合は `0` を返す。同じオブジェクトで再度呼ぶと、既存の流れに追加で流せる流量を返す。
- `min_cut(s)`: 残余グラフで `s` から到達できる頂点を `1` とした列を返す。

自己ループを追加できるが、最大流量には寄与しない。

## 時間計算量

$V$ を頂点数、$E$ を追加済みの辺数とする。

- `Dinic(n)`: $O(V)$
- `add_edge`: 償却 $O(1)$
- `bfs`: $O(V+E)$
- `dfs`: 1 回あたり最悪 $O(V+E)$。1 つのレベルグラフでブロッキングフローを作るための呼び出し全体で最悪 $O(VE)$
- `max_flow`: 一般の容量で $O(V^2 E)$
- `min_cut`: $O(V+E)$。長さ $V$ の到達判定列の出力を含む

## 空間計算量

- 残余グラフ、レベル・イテレータ、戻り値を含めて $O(V+E)$

## 注意点

`DinicEdge` は `from`, `to`, 初期容量 `cap`, 現在の流量 `flow` を持つ。公開メンバ `edges` には正辺の直後に逆辺が格納され、`add_edge` は正辺の番号を返す。公開メンバ関数 `bfs`, `dfs` はブロッキングフロー用の補助関数であり、通常は `max_flow` を使う。`dfs` は再帰で実装されており、再帰の深さは最大 $O(V)$ になる。

頂点数と容量は非負、各端点は頂点範囲内でなければならず、違反時は `runtime_error` を送出する。容量や流量の総和が `T` に収まることを前提とする。記載した計算量には引数検査と戻り値の構築を含む。
