---
title: BIRCH-Style Clustering Features (BIRCH型クラスタリング特徴量)
documentation_of: ../src/approximate/clustering/birch_features.hpp
---

BIRCHのclustering featureによる逐次要約と、容量超過時のcondensationを行う。完全な多段CF-treeではなく、boundedなleaf feature集合を提供するBIRCH相当のstreaming heuristicである。入力順に依存し、大域的なcluster最適性を保証しない。

## ClusteringFeature / BirchFeatureResult

各 `ClusteringFeature` は点数 `count`、`centroid`、各座標の母分散 `coordinate_variance`、その和 `squared_radius` を持つ。`BirchFeatureResult::labels[i]` は最終feature重心の最近傍へ点 `i` を割り当てたindexである。

## birch_clustering_features

~~~cpp
BirchFeatureResult birch_clustering_features(
    const DensePoints<Real>& points,
    long double threshold,
    size_t maximum_features
)
~~~

各点を最近傍featureへ仮追加し、新しいfeatureのRMS radiusが `threshold` 以下なら吸収する。吸収できなければsingleton featureを追加する。feature数が `maximum_features` を超えた場合は重心間距離が最小の2 featureをmergeする。この容量制約merge後はradiusが `threshold` を超えることがある。平均・分散は和と平方和ではなく安定な逐次式で更新する。

点数を $N$、次元を $D$、feature上限を $M$ とする。

- 時間計算量: $O(NM^2D)$
- 追加空間計算量: $O(MD+N)$

`threshold` は有限な非負値、`maximum_features` は正でなければならない。空入力を許す。平均・分散・距離を `long double` で表せない場合は `std::overflow_error` を送出する。
