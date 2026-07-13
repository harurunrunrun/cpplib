---
title: Lowest Common Ancestor
documentation_of: ../src/algorithm/tree/lca.hpp
---

二分 lifting による LCA。

## 関数

- `add_edge(u, v)`
- `build(root = 0)`
- `lca(u, v)`
- `dist(u, v)`
- `jump(v, k)`
    - `v` から根方向へ `k` 個進んだ頂点。
- `parent(v)`, `depth(v)`

## 時間計算量

- `LowestCommonAncestor(n)`: $O(N\log(N+1))$
- `size`: $O(1)$
- `add_edge`: 償却 $O(1)$
- `build`: $O(N\log(N+1))$
- `parent`, `depth`: $O(1)$
- `jump`, `lca`, `dist`: $O(\log(N+1))$

constructorはbinary lifting tableを確保・初期化し、`build` は木の検査とtable構築を行う。

## 空間計算量

- graphとbinary lifting tableを含めて $O(N\log(N+1))$
