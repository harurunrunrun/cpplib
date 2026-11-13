---
title: Diagonal Gaussian Mixture EM (対角共分散ガウス混合モデルEM法)
documentation_of: ../../../src/approximate/clustering/gaussian_mixture.hpp
---

対角共分散のガウス混合モデルを EM 法で推定する。EM 法は初期値に依存するヒューリスティックであり、尤度最大の大域解は保証しない。

## `GaussianMixtureResult`

- `weights[k]`: 混合比
- `means[k][d]`: 平均
- `variances[k][d]`: 対角成分の分散
- `responsibilities[i][k]`: 点 `i` が成分 `k` に属する事後確率（負担率）
- `labels[i]`: 負担率が最大の成分
- `log_likelihood`: 返したパラメータで計算し直した対数尤度
- `iterations`: 実行した M ステップの回数

## `gaussian_mixture_em`

~~~cpp
template<class Real, class URBG>
GaussianMixtureResult gaussian_mixture_em(
    const DensePoints<Real>& points,
    size_t component_count,
    URBG& generator,
    size_t maximum_iterations = 100,
    long double tolerance = 1e-7L,
    long double minimum_variance = 1e-9L
)
~~~

相異なる点番号をシャッフルして初期平均を選び、分散は全体の対角分散で初期化する。E ステップは log-sum-exp で計算し、M ステップでは逐次的な加重平均を使う。各分散は `minimum_variance` 以上に切り上げる。対数尤度の相対変化が `tolerance` 以下になると終了する。乱数生成器の状態が同じなら結果は決定的である。

点数を $N$、成分数を $K$、次元を $D$、実行した反復回数を $I$ とする。

- 時間計算量: $O((I+1)NKD)$（最後の E ステップを含む）
- 追加空間計算量: $O(ND+NK+KD)$

## 注意点

- 入力は空でなく、全点が同じ正の次元と有限の座標を持ち、`1 <= component_count <= N` でなければならない。
- `tolerance` は非負の有限値、`minimum_variance` は正の有限値でなければならず、満たさない場合は `std::invalid_argument` を送出する。
- 分散や尤度を `long double` で表せない場合、または全成分の確率が数値的にアンダーフローした場合は `std::overflow_error` を送出する。
