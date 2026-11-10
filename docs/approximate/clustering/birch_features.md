---
title: BIRCH-Style Clustering Features (BIRCH型クラスタリング特徴量)
documentation_of: ../../../src/approximate/clustering/birch_features.hpp
---

BIRCH のクラスタリング特徴量（CF）による逐次的な要約と、容量を超えたときの縮約を行う。完全な多段 CF 木ではなく、個数に上限のある葉の特徴量集合を保持する、BIRCH 相当のストリーミング型ヒューリスティックである。

## `ClusteringFeature` / `BirchFeatureResult`

各 `ClusteringFeature` は、点数 `count`、重心 `centroid`、各座標の母分散 `coordinate_variance`、その和 `squared_radius` を持つ。`BirchFeatureResult` は特徴量の列 `features` と `labels` を持ち、`labels[i]` は点 `i` を最終的な特徴量の重心のうち最も近いものへ割り当てたときの添字である。

## `birch_clustering_features`

~~~cpp
BirchFeatureResult birch_clustering_features(
    const DensePoints<Real>& points,
    long double threshold,
    size_t maximum_features
)
~~~

各点を最も近い特徴量へ仮に追加し、追加後の RMS 半径が `threshold` 以下なら吸収する。吸収できなければ、その点だけからなる特徴量を新たに追加する。特徴量の個数が `maximum_features` を超えた場合は、重心間の距離が最小の 2 つの特徴量をマージする。この容量制約によるマージの後では、半径が `threshold` を超えることがある。平均と分散は、和と平方和からではなく、数値的に安定な逐次更新式で計算する。

点数を $N$、次元を $D$、特徴量の上限を $M$ とする。

- 時間計算量: $O(NM^2D)$
- 追加空間計算量: $O(MD+N)$

## 注意点

- 点は全て同じ正の次元を持ち、座標は有限値でなければならない。空の入力は受理する。
- `threshold` は非負の有限値、`maximum_features` は正でなければならず、満たさない場合は `std::invalid_argument` を送出する。
- 平均・分散・距離を `long double` で表せない場合は `std::overflow_error` を送出する。
- 結果は入力順に依存し、大域的に最適なクラスタリングは保証しない。
