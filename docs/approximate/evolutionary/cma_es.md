---
title: Covariance Matrix Adaptation Evolution Strategy (共分散行列適応進化戦略)
documentation_of: ../../../src/approximate/evolutionary/cma_es.hpp
---

full covariance matrix、step-size path、covariance path を更新する box 制約付き
CMA-ES。The API is in `approximate::evolutionary` and minimizes by default.

## `CmaEsResult`

`individual`, `score`, final `mean`, full `covariance`, `step_size`,
`iterations`, `evaluations` を持つ。scalar member 参照は時間 $O(1)$。
個体・平均のコピーは $O(D)$、共分散のコピーは $O(D^2)$。

## `cma_es`

```cpp
cma_es(
    initial_mean, lower, upper, evaluate,
    generations, population_size, initial_step_size,
    rng, compare = {}
)
```

各世代で Cholesky 分解した共分散から population を標本化し、上位半数の
log weight で mean、二本の evolution path、rank-one/rank-$\mu$ covariance、
global step size を更新する。box 外の標本は座標ごとに clamp する。

次元 $D$、世代 $G$、population $L$、評価時間 $E$ に対する時間計算量は
$O(G(D^3+LD^2+LE+L\log L))$、追加空間計算量は $O(LD+D^2)$。
評価回数は正確に $GL$。

## 注意点

- mean と bounds は同じ非ゼロ次元、`lower <= mean <= upper`。
  世代は正、population は2以上、初期 step size は有限正。
- clamp が頻発すると正規分布更新に bias が入る。境界近傍では変数変換も検討する。
- Cholesky diagonal には machine epsilon の floor を入れるが、極端な ill
  conditioning では精度を保証しない。
- Stochastic convergence and global optimality are not guaranteed.
