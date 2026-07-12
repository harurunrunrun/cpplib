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

## 計算量

- `build`: $O(n \log n)$
- `lca`, `dist`, `jump`: $O(\log n)$
