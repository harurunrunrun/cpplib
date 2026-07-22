---
title: Min Cost Flow (最小費用流) [GRL_6_B]
documentation_of: ../src/algorithm/matching/flow/min_cost_flow.hpp
---

負辺を許す有向グラフで、指定量までのフローを最小費用で送る。
初回だけ Bellman–Ford 法でポテンシャルを作り、その後の各増加路は
reduced cost に対する Dijkstra 法で求める。

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

`MinCostFlowEdge` の `cap` は追加時の容量、`flow` は現在流れている量。
`MinCostFlowResult` はその呼出しで新たに送れた流量と追加費用を表す。

## コンストラクタ

```cpp
MinCostFlow<T> graph(n);
```

頂点数 `n` の残余グラフを作る。

## `add_edge`

```cpp
int id = graph.add_edge(from, to, cap, cost);
```

容量 `cap`、単位費用 `cost` の有向辺を追加し、publicな `edges` における
辺番号を返す。`cap` は非負でなければならない。自己ループも追加できる。

## `min_cost_flow`

```cpp
MinCostFlowResult<T> result =
    graph.min_cost_flow(source, sink, flow_limit, inf);
```

現在の残余グラフから、最大 `flow_limit` まで追加で流す。
これ以上流せない場合は送れた量だけを返す。呼出し後は残余グラフ、
`edges[id].flow` と容量が更新される。複数回呼ぶと前回の状態から続行する。

負費用辺を利用できる。`source` から到達可能な負閉路がある場合は
`runtime_error` を送出する。`flow_limit == 0` ならグラフを探索せず
`{0, 0}` を返す。

## `min_cost_for_exact_flow`

```cpp
std::optional<T> cost =
    graph.min_cost_for_exact_flow(source, sink, required_flow, inf);
```

`required_flow` を全て送れた場合は最小追加費用、送れなければ `nullopt` を
返す。`nullopt` の場合も、送れた分の残余状態と `edges[id].flow` は残る。

## 時間計算量

頂点数を $V$、追加辺数を $E$、1回の呼出しで成功する増加回数を $A$ とする。

- `MinCostFlow(n)`: $O(V)$
- `add_edge`: 償却 $O(1)$
- 初期ポテンシャル: $O(VE)$
- 各増加路: $O((V+E)\log V)$
- `min_cost_flow`, `min_cost_for_exact_flow`:
  $O(VE + A(V+E)\log V)$

到達部分で $E=\Omega(V)$ なら $O(VE + AE\log V)$ と書ける。
上界は容量値や総流量の数値の大きさには依存しない。

## 空間計算量

- グラフ、公開辺列、ポテンシャル、最短路作業領域を合わせて $O(V+E)$

## 注意点

`source` と `sink` は異なる有効頂点、各流量指定は非負でなければならない。
`inf` は実際に現れる有限距離より大きい値を指定する。費用加算、
reduced cost、`flow * cost` が `T` の表現範囲に収まることを前提とする。
`T` は加減算、乗算、大小比較と優先度付きキューでの順序比較が可能な型とする。
publicな `graph` は残余辺、`edges` は追加辺と現在流量を保持する。
