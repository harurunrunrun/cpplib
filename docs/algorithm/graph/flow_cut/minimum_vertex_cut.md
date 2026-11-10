---
title: Minimum Vertex Cut (最小頂点カット)
documentation_of: ../../../../src/algorithm/graph/flow_cut/minimum_vertex_cut.hpp
---

指定した 2 頂点を切り離すために削除する中間頂点の集合のうち、頂点容量の和が最小のものを求める。各頂点を入口と出口に分割したネットワーク上の最大流で解く。

## API

```cpp
struct MinimumVertexCutResult {
    long long capacity;
    std::vector<int> vertices;
};
```

- `capacity`: 削除する頂点の容量の和の最小値
- `vertices`: その最小頂点カットをなす頂点番号（昇順）

```cpp
std::optional<MinimumVertexCutResult> directed_minimum_vertex_cut(
    const std::vector<long long>& vertex_capacity,
    const std::vector<std::pair<int, int>>& directed_edges,
    int source,
    int sink
)
```

有向グラフで `source` から `sink` への最小頂点カットを求める。

```cpp
std::optional<MinimumVertexCutResult> undirected_minimum_vertex_cut(
    const std::vector<long long>& vertex_capacity,
    const std::vector<std::pair<int, int>>& undirected_edges,
    int source,
    int sink
)
```

無向グラフで `source` と `sink` の間の最小頂点カットを求める。

どちらも `source` と `sink` 自身は削除しない。中間頂点を削除するだけでは切り離せない場合（`source` から `sink` へ直接の辺がある場合など）は `std::nullopt` を返す。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- 頂点分割ネットワークの構築: $O(N + M)$
- Dinic 法による最大流: 一般の容量で $O(N^2 M)$
- 最小カットの復元: $O(N + M)$
- 各 API 全体: $O(N^2 M)$

## 空間計算量

$O(N + M)$

## 注意点

- 頂点容量は非負で、辺の端点と `source`, `sink` は $[0, N)$ の範囲内でなければならない。違反時は `runtime_error` を送出する。
- 頂点容量の和に番兵容量 $1$ を加えた値が `long long` に収まらない場合、または $N$ が `INT_MAX / 2` を超える場合は `overflow_error` を送出する。
- `source == sink` なら容量 $0$ の空集合を返す（このとき辺と頂点容量の検査は行わない）。
