---
title: Spectral Clustering and ISODATA (スペクトルクラスタリングとISODATA)
documentation_of: ../../../src/approximate/clustering/advanced_clustering.hpp
---

`approximate::clustering` 名前空間で、密な数値点群向けの二つの clustering
API を提供する。

## `spectral_clustering`

```cpp
spectral_clustering(
    points, cluster_count, bandwidth, random,
    maximum_eigensolver_sweeps = 50,
    maximum_kmeans_iterations = 100,
    tolerance = 1e-12L);
```

Gaussian affinity
$w_{ij}=\exp(-\lVert x_i-x_j\rVert^2/(2\sigma^2))$ と自己 loop を作り、
$D^{-1/2}WD^{-1/2}$ の上位固有ベクトルを cyclic Jacobi 法で求める。各行を
正規化した spectral embedding を k-means++ / Lloyd 法で分割する。

`SpectralClusteringResult` は `labels`、行正規化済み `embedding`、採用した
`eigenvalues`、k-means の `squared_error`、eigensolver と k-means の反復数を
返す。member 参照は $O(1)$。

点数を $N$、元次元を $D$、cluster 数を $K$、Jacobi sweep 数を $S$、k-means
反復数を $I$ とすると、時間計算量は
$O(N^2D+SN^3+INK^2)$、空間計算量は $O(N^2+NK)$。

## `isodata_clustering`

```cpp
isodata_clustering(
    points, initial_cluster_count,
    minimum_cluster_count, maximum_cluster_count,
    minimum_cluster_size,
    split_standard_deviation, merge_distance,
    random, maximum_iterations = 100);
```

k-means++ 中心から開始し、割当・中心更新、小 cluster の除去、近い中心の
併合、最大標準偏差軸での中心分割を反復する ISODATA。分割中心は選択軸上で
$\pm\sigma/2$ 移動する。

`IsodataResult` は最終 `centers`、`labels`、`squared_error`、反復・分割・併合
回数を返す。最大 cluster 数を $K$、反復数を $I$ とすると時間計算量は
$O(I(NKD+K^2D))$、空間計算量は $O(N+KD)$。

## 注意点

点は同じ正次元を持つ有限値でなければならない。bandwidth は正の有限値、
tolerance と ISODATA threshold は非負有限値でなければならない。cluster 数
境界、最小 cluster size、反復数の値域違反では `std::invalid_argument`。
Spectral Clustering は密な $N\times N$ 行列を保持するため大規模疎グラフ向け
ではない。両手法とも乱数 engine の状態と入力順が同じなら再現可能である。
