---
title: Min Cost Flow (最小費用流) [GRL_6_B]
documentation_of: ../../../../src/algorithm/matching/flow/min_cost_flow.hpp
---

負辺を許す有向グラフで、指定量までのフローを最小コストで流す。各呼び出しの最初に Bellman–Ford 法でポテンシャルを作り、その後の各増加路は被約コストに対する Dijkstra 法で求める。

## 型

```cpp
template<class T>
struct MinCostFlowEdge {
    int from;
    int to;
    T cap;
    T flow;
    T cost;
};

template<class T>
struct MinCostFlowResult {
    T flow;
    T cost;
};
```

`MinCostFlowEdge` の `cap` は追加時の容量、`flow` は現在流れている量である。`MinCostFlowResult` は、その呼び出しで新たに流せた流量と追加のコストを表す。

## コンストラクタ

```cpp
MinCostFlow<T> graph(n);
```

頂点数 `n` の残余グラフを作る。

## `add_edge`

```cpp
int id = graph.add_edge(from, to, cap, cost);
```

容量 `cap`、単位コスト `cost` の有向辺を追加し、公開メンバ `edges` における辺番号を返す。`cap` は非負でなければならない。自己ループも追加できる。

## `min_cost_flow`

```cpp
MinCostFlowResult<T> result =
    graph.min_cost_flow(source, sink, flow_limit, inf);
```

現在の残余グラフから、最大 `flow_limit` まで追加で流す。`flow_limit` と `inf` のデフォルトはともに `numeric_limits<T>::max() / 4` である。それ以上流せない場合は、流せた量だけを返す。呼び出し後は残余グラフと `edges[id].flow` が更新される。複数回呼ぶと前回の状態から続行する。

負コストの辺も利用できる。`source` から残余グラフで到達可能な負閉路がある場合は `runtime_error` を送出する。`flow_limit == 0` ならグラフを探索せずに `{0, 0}` を返す。

## `min_cost_for_exact_flow`

```cpp
std::optional<T> cost =
    graph.min_cost_for_exact_flow(source, sink, required_flow, inf);
```

`required_flow` をすべて流せた場合は最小の追加コストを、流せなければ `nullopt` を返す。`nullopt` の場合も、流せた分の残余グラフの状態と `edges[id].flow` は残る。

## 時間計算量

頂点数を $V$、追加した辺数を $E$、1 回の呼び出しで成功する増加の回数を $A$ とする。

- `MinCostFlow(n)`: $O(V)$
- `add_edge`: 償却 $O(1)$
- 初期ポテンシャル: $O(VE)$
- 各増加路: $O((V+E)\log V)$
- `min_cost_flow`, `min_cost_for_exact_flow`: $O(VE + A(V+E)\log V)$

到達部分で $E = \Omega(V)$ なら $O(VE + AE\log V)$ と書ける。上界は容量の値や総流量の数値の大きさには依存しない。

## 空間計算量

- グラフ、公開辺列、ポテンシャル、最短路の作業領域を合わせて $O(V+E)$

## 注意点

頂点数は非負、`source` と `sink` は相異なる有効な頂点、各流量の指定は非負でなければならず、違反時は `runtime_error` を送出する。`inf` には実際に現れる有限の距離より大きい値を指定する。コストの加算、被約コスト、`flow * cost` が `T` の表現範囲に収まることを前提とする。`T` は加減算、乗算、大小比較、および優先度付きキューでの順序比較が可能な型とする。公開メンバ `graph` は残余辺を、`edges` は追加した辺と現在の流量を保持する。
