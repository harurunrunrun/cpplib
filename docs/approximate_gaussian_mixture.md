---
title: Diagonal Gaussian Mixture EM (対角共分散ガウス混合モデルEM法)
documentation_of: ../src/approximate/clustering/gaussian_mixture.hpp
---

対角共分散Gaussian mixture modelをEM法で推定する。EMは初期値依存のheuristicであり、最大likelihoodの大域解を保証しない。

## GaussianMixtureResult

- `weights[k]`: mixture weight
- `means[k][d]`: 平均
- `variances[k][d]`: 対角分散
- `responsibilities[i][k]`: 点 `i` の事後確率
- `labels[i]`: responsibility最大のcomponent
- `log_likelihood`: 返却parameterで再計算した対数likelihood
- `iterations`: 実行したM-step数

## gaussian_mixture_em

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

異なる点番号をshuffleして初期平均を選び、全体の対角分散で初期化する。E-stepはlog-sum-expで計算し、M-stepでは逐次加重平均を使う。各分散を `minimum_variance` 以上にする。対数likelihoodの相対変化が `tolerance` 以下なら終了する。同じURBG状態では決定的である。

点数を $N$、component数を $K$、次元を $D$、実行iteration数を $I$ とする。

- 時間計算量: $O((I+1)NKD)$。最後のE-stepを含む
- 追加空間計算量: $O(ND+NK+KD)$

入力は非空で、`1 <= component_count <= N` を要求する。`tolerance` は有限な非負値、`minimum_variance` は有限な正値でなければならない。分散・likelihoodを `long double` で表せない場合、または全componentの確率が数値的にunderflowした場合は `std::overflow_error` を送出する。

## 注意点

座標列または距離callbackは、各APIで示した次元・有限性・非負性の条件を満たす必要がある。初期値や入力順に依存する手法では、明記した場合を除いて一意な分割や大域最適性を保証しない。
