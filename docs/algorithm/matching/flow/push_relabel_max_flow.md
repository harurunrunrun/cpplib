---
title: Push--Relabel Maximum Flow (Push--Relabel最大流)
documentation_of: ../../../../src/algorithm/matching/flow/push_relabel_max_flow.hpp
---

許容辺への `push` と頂点の高さの `relabel` を用い、relabel-to-front の順序で最大流を求める。

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

- コンストラクタ: 指定した頂点数の空のネットワークを作る
- `add_edge`: 非負の容量を持つ有向辺を追加し、順方向辺の番号を返す
- `max_flow`: プリフローを初期化し、relabel-to-front 法で最大流量を返す。`source == sink` なら $0$ を返す
- `min_cut`: 最終的な残余グラフで `source` から到達できる頂点を `1` とした列を返す

`edges` には順方向辺と残余逆辺が交互に格納される。

## 時間計算量

頂点数を $N$、入力の辺数を $M$ とする。

- コンストラクタ: $O(N)$
- `add_edge`: 償却 $O(1)$
- `max_flow`: $O(N^3 + NM)$（relabel-to-front 法の上界。単純グラフでは $O(N^3)$）
- `min_cut`: $O(N+M)$

## 空間計算量

$O(N+M)$。

## 注意点

`T` は符号付き整数型でなければならない。頂点数と容量は非負、頂点番号は範囲内でなければならず、違反時は `runtime_error` を送出する。始点から出る残余容量の和が `T` を超える場合は `overflow_error` を送出する。自己ループは最大流量に寄与しない。
