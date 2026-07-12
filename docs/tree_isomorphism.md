---
title: Tree Isomorphism
documentation_of: ../src/algorithm/tree/tree_isomorphism.hpp
---

AHU algorithm による木の同型性判定。

根付き木は根を固定して比較する。根なし木は center を根にして比較する。

## 関数

- `tree_centers(graph)`
    - 木の center を返す。個数は 1 または 2。
- `rooted_tree_isomorphism_labels(graph, root = 0)`
    - 同じ返り値内で、根付き部分木が同型なら同じ label。
- `rooted_tree_isomorphic(a, root_a, b, root_b)`
- `tree_isomorphic(a, b)`

## 計算量

- $O(n \log n)$
