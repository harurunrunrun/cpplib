---
title: Evolution Strategy (進化戦略)
documentation_of: ../src/approximate/evolutionary/evolution_strategy.hpp
---

`approximate::evolutionary`名前空間で、箱制約付き連続最小化に対する進化戦略を
提供する。個体は`std::vector<Real>`、上下限は同じ長さの`lower`, `upper`で表す。
`Real`は浮動小数点型でなければならない。初期個体は箱制約内でなければならず、
次元不一致、空の個体、不正な上下限では`std::invalid_argument`を送出する。
`Compare`を変更すれば最大化にも使える。

## `EvolutionStrategyResult`

```cpp
template<class Real, class Score>
struct EvolutionStrategyResult{
    std::vector<Real> individual;
    Score score;
    Real step_size;
    std::size_t iterations;
    std::size_t evaluations;
};
```

最良個体、目的値、終了時の変異幅、世代または反復数、目的関数評価回数を保持する。
各scalarメンバへのアクセスは$O(1)$、`individual`のコピーは$O(D)$である。

## `one_plus_one_es`

```cpp
auto result = one_plus_one_es(
    initial, lower, upper, evaluate, iterations,
    step_size, rng, success_multiplier, compare
);
```

各座標へ独立な正規乱数を加え、箱制約へclampした子を1個生成する。改善した子だけを
採用する。採用時は変異幅を`success_multiplier`倍し、不採用時は
`success_multiplier^(1/4)`で割る。既定値は1.5である。
変異幅は正の有限値、倍率は1以上の有限値でなければならない。

## `mu_plus_lambda_es`

```cpp
auto result = mu_plus_lambda_es(
    population, lower, upper, evaluate, generations,
    lambda, step_size, rng, compare
);
```

各世代で一様に選んだ親から`lambda`個の正規変異個体を作り、親`mu`個と子を
合わせた中から最良`mu`個を残す。`lambda == 0`では
`std::invalid_argument`を送出する。

## `mu_comma_lambda_es`

引数は`mu_plus_lambda_es`と同じで、次世代を子だけから選ぶ。
`lambda < mu`では`std::invalid_argument`を送出する。

## 計算量

$D$を次元、$E$を目的関数1回の時間、$G$を世代数とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `one_plus_one_es` | $O(E+\mathrm{iterations}(D+E))$ | $O(D)$ |
| `mu_plus_lambda_es` | $O(\mu E+G(\lambda(D+E)+(\mu+\lambda)\log(\mu+\lambda)))$ | $O((\mu+\lambda)D+\mu+\lambda)$ |
| `mu_comma_lambda_es` | $O(\mu E+G(\lambda(D+E)+\lambda\log\lambda))$ | $O(\lambda D+\lambda)$ |

## 注意点

目的関数と遺伝的操作は呼び出し中に有効で、比較関数は一貫した順序を与える必要がある。乱数結果の再現性には同じ乱数器状態・入力・標準ライブラリ実装が必要で、大域最適性は保証しない。
