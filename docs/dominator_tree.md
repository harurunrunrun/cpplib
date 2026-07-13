---
title: Dominator Tree
documentation_of: ../src/algorithm/graph/dominator_tree.hpp
---

Lengauer–Tarjan法により有向グラフのdominator treeを構築する。DFSとunion-findのpath compressionは反復処理で実装し、深いグラフでも再帰stackを使用しない。

# API

```cpp
DominatorTreeResult dominator_tree(
    const vector<vector<int>>& graph,
    int source
)
```

`source`から到達可能な各頂点のimmediate dominatorを求める。頂点番号が範囲外なら例外を送出する。

返り値は次を持つ。

```cpp
vector<int> immediate_dominator;
vector<vector<int>> tree;
vector<int> dfs_order;
```

- `immediate_dominator[v]`: `v`のimmediate dominator。`source`自身は`source`、到達不能頂点は`-1`
- `tree[v]`: dominator tree上での`v`の子
- `dfs_order`: `source`からのDFSで初めて訪問した頂点順

## 時間計算量

- $O((N+M)\alpha(N))$

## 空間計算量

- $O(N+M)$
