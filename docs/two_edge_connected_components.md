---
title: Two Edge Connected Components
documentation_of: ../src/algorithm/graph/two_edge_connected_components.hpp
---

無向グラフの二重辺連結成分分解。

橋を取り除いた後の連結成分を求める。

# 関数

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
