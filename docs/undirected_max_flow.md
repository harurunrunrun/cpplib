---
title: Undirected Maximum Flow (無向最大流) [FASTFLOW]
documentation_of: ../src/algorithm/graph/undirected_max_flow.hpp
---

容量付き無向グラフの2頂点間最大流を返す。

## UndirectedFlowEdge

```cpp
template<class T>
struct UndirectedFlowEdge {
    int from;
    int to;
    T capacity;
};
```

`from` と `to` を結ぶ容量 `capacity` の無向辺を表す。

## undirected_max_flow

```cpp
template<class T>
T undirected_max_flow(
    int n,
    const vector<UndirectedFlowEdge<T>>& edges,
    int source,
    int sink
);
```

`n` 頂点の無向グラフにおける `source` から `sink` への最大流量を返す。多重辺と自己辺を入力でき、自己辺は結果に影響しない。

## API別の時間計算量・空間計算量

$N=n$、$M=edges.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `undirected_max_flow` | $O(N^2M)$ | $O(N+M)$ |

## 注意点

- `n` は正で、両端点と `source`, `sink` は $[0,n)$ でなければならない。
- 容量は非負でなければならず、違反時は `runtime_error` を送出する。
- 最大流量と中間計算は `T` で表現できる必要がある。
