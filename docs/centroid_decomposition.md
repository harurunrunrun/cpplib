---
title: Centroid Decomposition
documentation_of: ../src/algorithm/tree/centroid_decomposition.hpp
---

木の重心分解。

元の木から重心を取り除く操作を再帰的に行い、重心木を作る。

## 関数

- `CentroidDecomposition(graph)`
- `root()`
    - 重心木の根。
- `parent(v)`
    - 重心木での親。根は `-1`。
- `depth(v)`
    - 重心木での深さ。
- `tree()`
    - 重心木の隣接リスト。

## 計算量

- 構築: $O(n \log n)$
