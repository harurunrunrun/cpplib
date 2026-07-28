---
title: Push--Relabel Maximum Flow (Push--Relabel最大流)
documentation_of: ../../../../src/algorithm/matching/flow/push_relabel_max_flow.hpp
---

許容辺への `push` と頂点高さの `relabel` を用い、relabel-to-front順序で最大流を求める。

## API

```cpp
template<signed_integral T>
struct PushRelabelEdge {
    int from;
    int to;
    T capacity;
    T flow;
};
```

```cpp
template<signed_integral T>
struct PushRelabel {
    int vertex_count;
    vector<PushRelabelEdge<T>> edges;
    vector<vector<int>> graph;

    explicit PushRelabel(int vertex_count);
    int add_edge(int from, int to, T capacity);
    T max_flow(int source, int sink);
    vector<char> min_cut(int source) const;
};
```

- コンストラクタ: 指定頂点数の空ネットワークを作る
- `add_edge`: 非負容量の有向辺を追加し、順方向辺IDを返す
- `max_flow`: preflowを初期化し、relabel-to-front法で最大流量を返す
- `min_cut`: 最終残余グラフで `source` から到達できる頂点を1で返す

`edges` には順方向辺と残余逆辺が交互に格納される。

## 時間計算量

頂点数を $N$、入力辺数を $M$ とする。

- コンストラクタ: $O(N)$
- `add_edge`: 償却 $O(1)$
- `max_flow`: $O(N^2M)$
- `min_cut`: $O(N+M)$

## 空間計算量

$O(N+M)$。

## 注意点

`T` は符号付き整数型で、容量は非負、頂点は範囲内でなければならない。違反時は `runtime_error` を送出する。始点から出る残余容量の和が `T` を超える場合は `overflow_error` を送出する。自己ループは最大流量に寄与しない。
