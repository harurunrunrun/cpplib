---
title: Strongly Connected Components (強連結成分分解)
documentation_of: ../src/algorithm/graph/strongly_connected_components.hpp
---

有向グラフの強連結成分分解。

# 関数

```cpp
strongly_connected_components(graph)
```

`StronglyConnectedComponentsResult` は次を持つ。

```cpp
int count;
vector<int> id;
vector<vector<int>> groups;
```

## 時間計算量

- `strongly_connected_components`: $O(N+M)$

逆グラフの構築、2回のDFS、全頂点を `groups` へ出力する時間を含む。

## 空間計算量

- 戻り値と逆グラフを含めて $O(N+M)$

## API契約・前提・例外

`id[v]` は成分id、`groups[id]` はその頂点列で `count == groups.size()`。成分idは縮約DAGのトポロジカル順になる。

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
