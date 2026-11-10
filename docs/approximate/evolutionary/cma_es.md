---
title: Covariance Matrix Adaptation Evolution Strategy (共分散行列適応進化戦略)
documentation_of: ../../../src/approximate/evolutionary/cma_es.hpp
---

完全な共分散行列、ステップサイズの進化パス、共分散の進化パスを更新する、箱型制約付きの CMA-ES である。`approximate::evolutionary` 名前空間にあり、デフォルトでは最小化を行う。

## `CmaEsResult`

最良の個体 `individual`、その評価値 `score`、最終的な平均 `mean`、完全な共分散行列 `covariance`、ステップサイズ `step_size`、世代数 `iterations`、評価回数 `evaluations` を持つ。スカラーのメンバの参照は時間 $O(1)$、個体・平均のコピーは $O(D)$、共分散のコピーは $O(D^2)$ である。

## `cma_es`

```cpp
cma_es(
    initial_mean, lower, upper, evaluate,
    generations, population_size, initial_step_size,
    rng, compare = {}
)
```

各世代で、Cholesky 分解した共分散から集団を標本化する。上位半数を対数重みで重み付けして、平均、2 本の進化パス、rank-one / rank-$\mu$ の共分散、全体のステップサイズを更新する。箱の外に出た標本は座標ごとに範囲内へ切り詰める。

次元を $D$、世代数を $G$、集団サイズを $L$、評価 1 回の時間を $E$ とすると、時間計算量は $O(G(D^3+LD^2+LE+L\log L))$、追加空間計算量は $O(LD+D^2)$ である。評価回数はちょうど $GL$ 回である。

## 注意点

- 平均と境界は同じ正の次元を持ち、`lower <= initial_mean <= upper` でなければならない。世代数は正、集団サイズは 2 以上、初期ステップサイズは正の有限値でなければならない。違反時は `std::invalid_argument` を送出する。
- 切り詰めが頻発すると、正規分布の更新に偏りが生じる。境界付近が重要なら変数変換も検討する。
- Cholesky 分解の対角成分には machine epsilon の下限を入れるが、極端に悪条件な場合の精度は保証しない。
- 確率的な手法であり、収束と大域最適性は保証しない。
