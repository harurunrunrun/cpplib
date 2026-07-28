---
title: Capacity-Scaling Maximum Flow (容量スケーリング最大流)
documentation_of: ../../../../src/algorithm/matching/flow/capacity_scaling_max_flow.hpp
---

残余容量の上位bitから順に有効化する容量スケーリング法で最大流を求める。

## API

```cpp
template<signed_integral T>
struct CapacityScalingMaxFlowEdge {
    int from;
    int to;
    T capacity;
    T flow;
};
```

```cpp
template<signed_integral T>
struct CapacityScalingMaxFlow {
    int vertex_count;
    vector<CapacityScalingMaxFlowEdge<T>> edges;
    vector<vector<int>> graph;

    explicit CapacityScalingMaxFlow(int vertex_count);
    int add_edge(int from, int to, T capacity);
    T max_flow(int source, int sink);
    vector<char> min_cut(int source) const;
};
```

- コンストラクタ: `vertex_count` 頂点の空ネットワークを作る
- `add_edge`: 有向辺を追加し、`edges` における順方向辺IDを返す
- `max_flow`: 現在の残余ネットワークへ追加できる最大流量を返す
- `min_cut`: 最後の流れに対し、残余グラフで `source` から到達できる頂点を1で返す

`edges` には順方向辺と残余逆辺が交互に格納される。

## 時間計算量

頂点数を $N$、残余辺を含めない入力辺数を $M$、最大容量を $U$ とする。

- コンストラクタ: $O(N)$
- `add_edge`: 償却 $O(1)$
- `max_flow`: $O(M^2\log(U+1))$
- `min_cut`: $O(N+M)$

## 空間計算量

$O(N+M)$。

## 注意点

`T` は符号付き整数型でなければならない。容量は非負、頂点は範囲内でなければならず、違反時は `runtime_error` を送出する。最大流量が `T` を超える場合は `overflow_error` を送出する。同じオブジェクトで再度 `max_flow` を呼ぶと、既存の流れから追加できる流量を返す。
