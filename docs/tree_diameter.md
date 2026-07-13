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

## 時間計算量

$L$ を返す直径pathの頂点数とする。

- `tree_diameter`: $O(N+L)=O(N)$
- `weighted_tree_diameter`: $O(N+L)=O(N)$

どちらも木の検査、2回の全頂点走査、path復元を含む。

## 空間計算量

- 距離・親・stackと出力pathを含めて $O(N)$
