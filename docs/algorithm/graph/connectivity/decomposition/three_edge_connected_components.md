---
title: Three-Edge-Connected Components (三辺連結成分分解) [three_edge_connected_components]
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/three_edge_connected_components.hpp
---

無向多重グラフを三辺連結成分へ線形時間で分解する。2 頂点は、任意の高々 2 本の辺を削除しても同じ連結成分に残るとき、同じ三辺連結成分に属する。

## 結果

```cpp
struct ThreeEdgeConnectedComponentsResult {
    int count;
    vector<int> id;
    vector<vector<int>> groups;
};
```

- `count` は成分の個数。
- `id[v]` は頂点 `v` が属する成分の番号。
- `groups[c]` は成分 `c` の全頂点。`groups` は全頂点を重複なく分割する。

## three_edge_connected_components

```cpp
ThreeEdgeConnectedComponentsResult three_edge_connected_components(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

自己ループと多重辺を許す。自己ループは異なる頂点間の辺連結度に寄与しないため、分解中は無視する。非連結グラフにも使える。DFS を明示的なスタックで行うため、深いグラフでもコールスタックを消費しない。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- $O(N + M)$

## 空間計算量

- $O(N + M)$

## 注意点

`vertex_count` が負、または辺の端点が範囲外の場合は `std::runtime_error` を送出する。
