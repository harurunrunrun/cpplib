---
title: Three-Edge-Connected Components (三辺連結成分分解)
documentation_of: ../src/algorithm/graph/three_edge_connected_components.hpp
---

無向多重グラフを3-edge-connected componentsへ線形時間で分解する。2頂点は、任意の高々2辺を削除しても同じconnected componentに残るとき、同じ3-edge-connected componentに属する。

# 結果

```cpp
struct ThreeEdgeConnectedComponentsResult {
    int count;
    vector<int> id;
    vector<vector<int>> groups;
};
```

- `count` はcomponent数。
- `id[v]` は頂点 `v` が属するcomponent番号。
- `groups[c]` はcomponent `c` の全頂点。全頂点を重複なく分割する。

# 3-edge-connected components

```cpp
ThreeEdgeConnectedComponentsResult three_edge_connected_components(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

自己loopと多重辺を許す。自己loopは異なる頂点間のedge connectivityに寄与しないため、分解中は無視する。非連結グラフにも利用できる。DFSを明示stackで実行し、深いグラフでもcall stackを消費しない。

## 時間計算量

- `O(N+M)`

## 空間計算量

- `O(N+M)`

# 例外

`vertex_count`または辺端点が範囲外の場合は `runtime_error` を送出する。
