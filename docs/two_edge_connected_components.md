---
title: Two Edge Connected Components (二重辺連結成分分解) [two_edge_connected_components]
documentation_of: ../src/algorithm/graph/connectivity/two_edge_connected_components.hpp
---

無向グラフの二重辺連結成分分解。

橋を取り除いた後の連結成分を求める。

## 関数

```cpp
two_edge_connected_components(n, edges)
```

`TwoEdgeConnectedComponentsResult` は次を持つ。

```cpp
int count;
vector<int> id;
vector<vector<int>> groups;
vector<int> bridges;
```

`bridges` には入力辺の id が入る。

## 時間計算量

$N$ を頂点数、$M$ を辺数とする。

- lowlinkと成分DFS: $O(N+M)$
- 橋IDの整列: $O(M\log(M+1))$
- `two_edge_connected_components` 全体: $O(N+M\log(M+1))$

全頂点を `groups` へ出力する時間を含む。

## 空間計算量

- 戻り値と隣接リストを含めて $O(N+M)$

## 注意点

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
