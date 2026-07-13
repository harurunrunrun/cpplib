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
距離表とLance--Williams更新を用いるため時間計算量は $O(n^3)$、追加空間計算量は $O(n^2)$。
`n==0`では空のdendrogramを返す。入力距離は有限かつ非負でなければならず、
違反時は`std::invalid_argument`を送出する。

## 注意点

座標列または距離callbackは、各APIで示した次元・有限性・非負性の条件を満たす必要がある。初期値や入力順に依存する手法では、明記した場合を除いて一意な分割や大域最適性を保証しない。
