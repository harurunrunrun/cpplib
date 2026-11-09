---
title: Undirected Maximum Flow (無向最大流) [FASTFLOW]
documentation_of: ../../../../src/algorithm/graph/flow_cut/undirected_max_flow.hpp
---

容量付き無向グラフの 2 頂点間の最大流量を Dinic 法で求める。

## `UndirectedFlowEdge`

```cpp
template <class T>
struct UndirectedFlowEdge {
    int from;
    int to;
    T capacity;
};
```

`from` と `to` を結ぶ容量 `capacity` の無向辺を表す。

## `undirected_max_flow`

```cpp
template <class T>
T undirected_max_flow(
    int n,
    const std::vector<UndirectedFlowEdge<T>>& edges,
    int source,
    int sink
);
```

`n` 頂点の無向グラフにおける `source` から `sink` への最大流量を返す。多重辺と自己ループを入力でき、自己ループは結果に影響しない。`source == sink` のときは $0$ を返す。

## 計算量

$N = n$、$M$ を `edges` の要素数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `undirected_max_flow` | $O(N^2 M)$ | $O(N + M)$ |

## 注意点

- `source`, `sink` と各辺の両端点は $[0, n)$ の範囲内でなければならない。違反時は `runtime_error` を送出する。
- 容量は非負でなければならず、違反時は `runtime_error` を送出する。
- `T` は算術型でなければならない。最大流量と途中の計算結果は `T` で表現できなければならない。
- 内部の `Dinic::max_flow` をデフォルトの流量上限 `std::numeric_limits<T>::max() / 4` で呼ぶため、真の最大流量がこれを超える場合は上限値で打ち切られる。
