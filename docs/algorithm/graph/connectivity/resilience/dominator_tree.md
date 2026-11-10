---
title: Dominator Tree (支配木) [dominatortree]
documentation_of: ../../../../../src/algorithm/graph/connectivity/resilience/dominator_tree.hpp
---

Lengauer–Tarjan 法により、有向グラフの支配木（dominator tree）を構築する。DFS と Union-Find の経路圧縮は反復処理で実装しており、深いグラフでも再帰スタックを使わない。

## API

```cpp
DominatorTreeResult dominator_tree(
    const vector<vector<int>>& graph,
    int source
)
```

`graph[v]` は頂点 `v` から出る辺の行き先の列である。`source` から到達可能な各頂点の直接支配点（immediate dominator）を求める。

戻り値は次のメンバを持つ。

```cpp
vector<int> immediate_dominator;
vector<vector<int>> tree;
vector<int> dfs_order;
```

- `immediate_dominator[v]`: `v` の直接支配点。`source` 自身は `source`、到達不能な頂点は $-1$
- `tree[v]`: 支配木における `v` の子の列
- `dfs_order`: `source` からの DFS で頂点を初めて訪れた順（行きがけ順）に並べた列

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- $O((N + M) \log N)$

経路圧縮のみを行う単純版の Lengauer–Tarjan 法であり、ランクによる併合は行わない。

## 空間計算量

- $O(N + M)$

## 注意点

- `source` と各辺の行き先は $[0, N)$ でなければならない。違反時は `std::runtime_error` を送出する。
- 自己ループと多重辺を許す。
- 戻り値の頂点番号も入力と同じ 0-indexed である。
