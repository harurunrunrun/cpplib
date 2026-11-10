---
title: Distribution Estimation Algorithms (分布推定アルゴリズム)
documentation_of: ../../../src/approximate/evolutionary/distribution_estimation.hpp
---

集団の明示的な交叉ではなく、優良個体から標本分布を更新する分布推定アルゴリズム（EDA）群である。どの関数もデフォルトでは最小化を行い、外部から渡された乱数生成器を使う。

## `DistributionEstimationResult`

最良の二値個体 `individual`、その評価値 `score`、最終的なベルヌーイ分布の確率 `probabilities`、世代数 `iterations`、評価回数 `evaluations` を持つ。スカラーのメンバへのアクセスは $O(1)$、2 つの `std::vector` のコピーはビット数 $B$ に対して $O(B)$ である。

## `estimation_of_distribution_algorithm`

```cpp
estimation_of_distribution_algorithm(
    parameters, sample, update, evaluate,
    generations, population_size, elite_count, rng, compare = {}
)
```

`sample(parameters, rng)` で集団を生成し、上位の優良個体を `update(elites, parameters)` に渡す、問題に依存しない汎用の EDA である。世代数を $G$、集団サイズを $N$、標本化 1 回のコストを $S$、評価 1 回のコストを $E$、更新 1 回のコストを $U$ とすると、時間計算量は $O(G(N(S+E)+N\log N+U))$、追加空間計算量は個体 $O(N)$ 個分である。

## `univariate_marginal_distribution_algorithm`

```cpp
univariate_marginal_distribution_algorithm<Real>(
    bit_count, evaluate, generations, population_size,
    elite_count, rng, margin = 0, compare = {}
)
```

優良個体の各ビットの平均を次世代のベルヌーイ確率とする UMDA である。確率は `[margin, 1 - margin]` に収める。時間計算量は $O(G(N(B+E)+N\log N))$、追加空間計算量は $O(NB)$ である。

## `population_based_incremental_learning`

```cpp
population_based_incremental_learning<Real>(
    bit_count, evaluate, generations, population_size,
    learning_rate, mutation_probability, mutation_shift,
    rng, compare = {}
)
```

その世代の最良個体のビットへ確率を学習率 `learning_rate` だけ近づけ、その後、各ビットを確率 `mutation_probability` でランダムなビットの方向へ `mutation_shift` だけずらす PBIL である。時間計算量は $O(GN(B+E))$、追加空間計算量は $O(B)$ である。

## 注意点

- ビット数・世代数・集団サイズ・優良個体数は正で、`elite_count <= population_size` でなければならない。違反時は `std::invalid_argument` を送出する。
- UMDA の `margin` は $[0, 0.5)$ の有限値でなければならない。PBIL の 3 つの率は $[0,1]$ の有限値で、学習率だけは正でなければならない。
- 汎用 EDA の `update` は、次の標本化に有効なパラメータを返す責任を負う。
- 評価回数は、汎用 EDA・UMDA・PBIL のいずれもちょうど $GN$ 回である。
- 各変数の周辺分布を独立に扱うため、変数間の依存関係を表現できない。そのため compact GA・UMDA・PBIL は、エピスタシスの強い目的関数では停滞しうる。
