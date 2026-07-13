---
title: Agglomerative Hierarchical Clustering (凝集型階層クラスタリング)
documentation_of: ../src/approximate/clustering/hierarchical_clustering.hpp
---

`approximate::clustering::Linkage`は`single`、`complete`、`average`を持つ。
`HierarchicalMerge{left,right,distance,size}`は結合したcluster ID、結合距離、
結合後サイズを表す。`HierarchicalClusteringResult::merges`では葉を`0..n-1`、
結合`i`で作るclusterを`n+i`とする。

## `agglomerative_clustering`

```cpp
auto result = agglomerative_clustering(n, distance, linkage);
```

最小linkage距離のcluster対を反復結合する。同距離ではID対の辞書順を使う。
距離表とLance--Williams更新を用いるため時間$O(n^3)$、追加領域$O(n^2)$。
`n==0`では空のdendrogramを返す。入力距離は有限かつ非負でなければならず、
違反時は`std::invalid_argument`を送出する。
