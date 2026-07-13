---
title: Complement Shortest Path
documentation_of: ../src/algorithm/graph/complement_shortest_path.hpp
---

補グラフ上の単一始点無重み最短路。

`graph[v]` に元のグラフの辺を入れる。補グラフでは、`v != u` かつ `v -> u` が元のグラフに存在しないときに辺がある。

# 関数

```cpp
complement_shortest_path(graph, source)
```

`ComplementShortestPathResult` は次を持つ。

```cpp
vector<int> dist;
vector<int> parent;
```

到達不能な頂点の `dist` は `-1`。

## 時間計算量

- `complement_shortest_path`: $O(N^2+M)$

元グラフの全辺検査と、未到達頂点列の反復走査、長さ $N$ の結果列構築を含む。

## 空間計算量

- 戻り値と探索領域を含めて $O(N)$

## API契約・前提・例外

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
