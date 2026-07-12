---
title: Tree Centroid
documentation_of: ../src/algorithm/tree/tree_centroid.hpp
---

木の重心。

重心は、取り除いたときに残る各連結成分のサイズがすべて $n/2$ 以下になる頂点。

## 関数

- `tree_centroids(graph)`
    - 重心をすべて返す。個数は 1 または 2。
- `tree_centroid(graph)`
    - 先頭の重心を返す。空木なら `-1`。

## 計算量

- $O(n)$
