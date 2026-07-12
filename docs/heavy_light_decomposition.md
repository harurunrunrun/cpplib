---
title: Heavy Light Decomposition
documentation_of: ../src/algorithm/tree/heavy_light_decomposition.hpp
---

木を heavy path に分解し、パス・部分木を Euler Tour 上の区間に移す。

`path_query(u, v, vertex)` は `[l, r)` の列を返す。辺に値を持つ場合は `vertex = false` にして、LCA に対応する頂点を除く。

## 関数

- `add_edge(u, v)`
- `build(root = 0)`
- `lca(u, v)`
- `dist(u, v)`
- `jump(v, k)`
    - `v` から根方向へ `k` 個進んだ頂点。
- `is_ancestor(u, v)`
- `path_query(u, v, vertex = true)`
- `subtree_query(v, vertex = true)`
- `in(v)`, `out(v)`, `operator[](k)`

## 計算量

- `build`: $O(n)$
- `lca`, `dist`, `jump`, `path_query`: $O(\log n)$
- その他: $O(1)$
