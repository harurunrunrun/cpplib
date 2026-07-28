---
title: Minimum Vertex Cut (最小頂点カット)
documentation_of: ../../../../src/algorithm/graph/flow_cut/minimum_vertex_cut.hpp
---

指定した2頂点を切り離すために削除する、中間頂点の容量和が最小の集合を求める。

## API

```cpp
struct MinimumVertexCutResult {
    long long capacity;
    vector<int> vertices;
};
```

- `capacity`: 削除する頂点容量の最小和
- `vertices`: その最小頂点カットをなす頂点番号。昇順

```cpp
optional<MinimumVertexCutResult> directed_minimum_vertex_cut(
    const vector<long long>& vertex_capacity,
    const vector<pair<int, int>>& directed_edges,
    int source,
    int sink
)
```

有向グラフの最小 `source`--`sink` 頂点カットを求める。

```cpp
optional<MinimumVertexCutResult> undirected_minimum_vertex_cut(
    const vector<long long>& vertex_capacity,
    const vector<pair<int, int>>& undirected_edges,
    int source,
    int sink
)
```

無向グラフの最小 `source`--`sink` 頂点カットを求める。

どちらも `source` と `sink` 自身は削除しない。中間頂点だけでは切り離せない場合は `nullopt` を返す。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- 頂点分割ネットワークの構築: $O(N+M)$
- Dinic法による最大流: 一般容量で $O(N^2M)$
- 最小カットの復元: $O(N+M)$
- 各API全体: $O(N^2M)$

## 空間計算量

$O(N+M)$。

## 注意点

頂点容量は非負で、辺の端点と `source`, `sink` は頂点範囲内でなければならない。違反時は `runtime_error` を送出する。容量和に番兵容量を加えられない場合は `overflow_error` を送出する。`source == sink` なら容量0の空集合を返す。
