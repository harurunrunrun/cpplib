---
title: Tree Diameter
documentation_of: ../src/algorithm/tree/tree_diameter.hpp
---

木の直径。

端点、長さ、端点間のパスを返す。重み付き版は辺重みが非負であることを仮定する。

## 関数

- `tree_diameter(graph)`
    - 無重み木。長さは辺数。
- `weighted_tree_diameter<T>(graph)`
    - 重み付き木。

## 計算量

- $O(n)$
