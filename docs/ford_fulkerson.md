---
title: Ford Fulkerson (フォード・ファルカーソン法)
documentation_of: ../src/algorithm/matching/ford_fulkerson.hpp
---

Ford-Fulkerson法による最大流。

# コンストラクタ

```cpp
FordFulkerson<T> graph(n)
```

# 関数

```cpp
graph.add_edge(from, to, cap)
graph.max_flow(s, t, flow_limit)
```

容量が整数で、最大流量が小さい場合に使う。

`flow_limit` は非負である必要がある。`s == t` の場合は `0` を返す。

self-loop を追加できるが、最大流量には寄与しない。

## 時間計算量

$V$ を頂点数、$E$ を追加済み辺数、$A$ を今回の `max_flow` で成功する増加回数とする。

- `FordFulkerson(n)`: $O(V)$
- `add_edge`: 償却 $O(1)$
- `dfs`: 1回 $O(V+E)$
- `max_flow`: $O((A+1)(V+E))$

整数容量で正の容量が1以上なら $A$ は追加流量以下なので、追加流量を $F$ として $O((F+1)(V+E))$ でも上から抑えられる。

## 空間計算量

- 残余グラフとDFS領域を含めて $O(V+E)$

## 注意点

`FordFulkersonEdge` は `to`, 逆辺index `rev`, 残余 `cap` を持つ。public `dfs` はその探索用に `used` 初期化済みであることを前提とし、通常は `max_flow` を使う。複数回呼出しは現在の残余graphから続行する。

頂点数は非負、各端点は対応する頂点範囲内でなければならず、違反時は `runtime_error`。容量・cost・総和の演算結果が `T` に収まることを前提とする。記載した計算量には引数検査とResultの構築を含む。
