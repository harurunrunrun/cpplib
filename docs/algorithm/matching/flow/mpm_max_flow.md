---
title: MPM Maximum Flow (MPM最大流)
documentation_of: ../../../../src/algorithm/matching/flow/mpm_max_flow.hpp
---

各レベルグラフで頂点ポテンシャルが最小の頂点を選び、前後へまとめて流すMalhotra--Pramodh--Maheshwari法で最大流を求める。

## API

```cpp
MPMMaxFlow<Capacity> flow(vertex_count);
```

空の有向ネットワークを構築する。`Capacity` は64-bit以下の `bool` 以外の整数型とする。

```cpp
int edge_id = flow.add_edge(from, to, capacity);
```

非負容量の辺を追加し、内部辺番号を返す。

```cpp
Capacity value = flow.max_flow(source, sink, flow_limit);
```

現在の残余ネットワークへ `flow_limit` まで追加で流す。省略時の上限は `numeric_limits<Capacity>::max() / 4`。

```cpp
vector<unsigned char> side = flow.min_cut(source);
```

残余グラフで `source` から到達可能な頂点を `1` とする最小カット側を返す。

## 時間計算量

頂点数を $V$、辺数を $E$ とする。

- 構築、`add_edge`: 償却 $O(1)$
- `max_flow`: $O(V^3+E)$
- `min_cut`: $O(V+E)$

## 空間計算量

$O(V+E)$。

## 注意点

頂点番号の範囲違反または負容量では `runtime_error` を送出する。容量和と返す流量が `Capacity` の範囲内であることを前提とし、ポテンシャル和は `__int128` で保持する。
