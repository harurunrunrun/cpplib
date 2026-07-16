---
title: Two Vertex Connected Components (二重頂点連結成分分解) [biconnected_components]
documentation_of: ../src/algorithm/graph/two_vertex_connected_components.hpp
---

無向グラフの二重頂点連結成分分解。

## 関数

```cpp
two_vertex_connected_components(n, edges)
```

`TwoVertexConnectedComponentsResult` は次を持つ。

```cpp
int count;
vector<int> articulation;
vector<vector<int>> vertex_groups;
vector<vector<int>> edge_groups;
```

`edge_groups` には入力辺の id が入る。

孤立点は1頂点の成分として返す。

## 時間計算量

$N$ を頂点数、$M$ を辺数とする。

- `two_vertex_connected_components`: $O(N+M\log(M+1))$

lowlinkとDFSは $O(N+M)$。各成分の頂点・辺IDを整列する合計時間が $O(M\log(M+1))$。孤立点を含む戻り値の総要素数は $O(N+M)$。

## 空間計算量

- 戻り値を含めて $O(N+M)$

## 注意点

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
