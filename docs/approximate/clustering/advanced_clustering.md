---
title: Spectral Clustering and ISODATA (スペクトルクラスタリングとISODATA)
documentation_of: ../../../src/approximate/clustering/advanced_clustering.hpp
---

`approximate::clustering` 名前空間で、密な数値点群に対する 2 つのクラスタリング API を提供する。

## `spectral_clustering`

```cpp
spectral_clustering(
    points, cluster_count, bandwidth, random,
    maximum_eigensolver_sweeps = 50,
    maximum_kmeans_iterations = 100,
    tolerance = 1e-12L);
```

ガウス型の類似度 $w_{ij}=\exp(-\lVert x_i-x_j\rVert^2/(2\sigma^2))$（$\sigma$ は `bandwidth`）と自己ループを持つ類似度行列 $W$ を作り、$D^{-1/2}WD^{-1/2}$ の上位固有ベクトルを巡回 Jacobi 法で求める。各行を正規化したスペクトル埋め込みを k-means++ による初期化と Lloyd 法で分割する。

戻り値の `SpectralClusteringResult` は、`labels`、行正規化済みの `embedding`、採用した固有値 `eigenvalues`、k-means の二乗誤差 `squared_error`、固有値計算の sweep 数 `eigensolver_sweeps`、k-means の反復数 `kmeans_iterations` を持つ。各メンバの参照は $O(1)$ である。

点数を $N$、元の次元を $D$、クラスタ数を $K$、Jacobi 法の sweep 数を $S$、k-means の反復数を $I$ とすると、時間計算量は $O(N^2D+SN^3+INK^2)$、空間計算量は $O(N^2+NK)$ である。

## `isodata_clustering`

```cpp
isodata_clustering(
    points, initial_cluster_count,
    minimum_cluster_count, maximum_cluster_count,
    minimum_cluster_size,
    split_standard_deviation, merge_distance,
    random, maximum_iterations = 100);
```

k-means++ で選んだ中心から始め、割当と中心の更新、小さいクラスタの除去、近い中心の併合、標準偏差が最大の軸での中心の分割を反復する ISODATA 法である。分割した中心は選んだ軸の上で $\pm\sigma/2$ だけ移動する。

戻り値の `IsodataResult` は、最終的な中心 `centers`、`labels`、`squared_error`、反復回数 `iterations`、分割回数 `splits`、併合回数 `merges` を持つ。最大クラスタ数を $K$、反復数を $I$ とすると、時間計算量は $O(I(NKD+K^2D))$、空間計算量は $O(N+KD)$ である。

## 注意点

- 点は全て同じ正の次元を持ち、座標は有限値でなければならない。
- `bandwidth` は正の有限値、`tolerance` と ISODATA の閾値（`split_standard_deviation`、`merge_distance`）は非負の有限値でなければならない。
- クラスタ数が $[1, N]$ にない場合、ISODATA のクラスタ数の範囲が `1 <= minimum_cluster_count <= initial_cluster_count <= maximum_cluster_count <= N` を満たさない場合、`minimum_cluster_size == 0`、`maximum_eigensolver_sweeps == 0` の場合は `std::invalid_argument` を送出する。`bandwidth` に関する計算がオーバーフローした場合は `std::overflow_error` を送出する。
- スペクトルクラスタリングは密な $N\times N$ 行列を保持するため、大規模な疎グラフには向かない。
- どちらの手法も、乱数エンジンの状態と入力順が同じなら結果は再現できる。
