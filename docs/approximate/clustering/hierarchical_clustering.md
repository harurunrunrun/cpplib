---
title: Agglomerative Hierarchical Clustering (凝集型階層クラスタリング)
documentation_of: ../../../src/approximate/clustering/hierarchical_clustering.hpp
---

`approximate::clustering::Linkage` は `single`、`complete`、`average` を持つ。`HierarchicalMerge{left, right, distance, size}` は、結合した 2 つのクラスタの ID、結合距離、結合後のサイズを表す。`HierarchicalClusteringResult::merges` では葉を `0` から `n-1`、`i` 番目の結合で作られるクラスタを `n+i` とする。

## `agglomerative_clustering`

```cpp
auto result = agglomerative_clustering(n, distance, linkage);
```

連結距離が最小のクラスタ対を繰り返し結合し、`merges` に $n-1$ 回の結合を格納する。同距離の場合は ID の対の辞書順で選ぶ。距離表と Lance–Williams の更新式を用いるため、時間計算量は $O(n^3)$、追加空間計算量は $O(n^2)$ である。`n == 0` では空のデンドログラムを返す。

## 注意点

- 入力距離 `distance(i, j)` は有限かつ非負でなければならず、違反時や未知の `linkage` では `std::invalid_argument` を送出する。
- `2n-1` が `size_t` に収まらないほど `n` が大きい場合は `std::length_error` を送出する。
- 貪欲な結合であり、指定したクラスタ数に対する大域最適性は保証しない。
