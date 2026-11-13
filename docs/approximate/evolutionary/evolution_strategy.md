---
title: Evolution Strategy (進化戦略)
documentation_of: ../../../src/approximate/evolutionary/evolution_strategy.hpp
---

`approximate::evolutionary` 名前空間で、箱型制約付きの連続最小化問題に対する進化戦略を提供する。個体は `std::vector<Real>`、上下限は同じ長さの `lower`, `upper` で表す。`Real` は浮動小数点型でなければならない。`Compare` を変更すれば最大化にも使える。

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

最良の個体、その目的値、終了時の変異幅、世代数または反復回数、目的関数の評価回数を持つ。各スカラーのメンバへのアクセスは $O(1)$、`individual` のコピーは $O(D)$ である。

## `one_plus_one_es`

```cpp
auto result = one_plus_one_es(
    initial, lower, upper, evaluate, iterations,
    step_size, rng, success_multiplier = 1.5, compare = {}
);
```

各座標に独立な正規乱数を加え、箱型制約の範囲内に切り詰めた子を 1 個生成する。改善した子だけを採用する。採用したときは変異幅を `success_multiplier` 倍し、採用しなかったときは、`success_multiplier` を $m$ として $m^{1/4}$ で割る。

## `mu_plus_lambda_es`

```cpp
auto result = mu_plus_lambda_es(
    population, lower, upper, evaluate, generations,
    lambda, step_size, rng, compare = {}
);
```

各世代で、一様に選んだ親から `lambda` 個の正規変異個体を作り、`mu` 個（初期集団のサイズ）の親と子を合わせた中から最良の `mu` 個を残す。

## `mu_comma_lambda_es`

引数は `mu_plus_lambda_es` と同じで、次世代を子だけから選ぶ。

## 計算量

次元を $D$、目的関数 1 回の時間を $E$、世代数を $G$、`iterations` を $I$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `one_plus_one_es` | $O(E+I(D+E))$ | $O(D)$ |
| `mu_plus_lambda_es` | $O(\mu E+G(\lambda(D+E)+(\mu+\lambda)\log(\mu+\lambda)))$ | $O((\mu+\lambda)D+\mu+\lambda)$ |
| `mu_comma_lambda_es` | $O(\mu E+G(\lambda(D+E)+\lambda\log\lambda))$ | $O(\lambda D+\lambda)$ |

## 注意点

- 初期個体は箱型制約の範囲内でなければならない。次元の不一致、空の個体、不正な上下限では `std::invalid_argument` を送出する。
- 変異幅 `step_size` は正の有限値、`success_multiplier` は 1 以上の有限値でなければならない。
- `lambda == 0`、および `mu_comma_lambda_es` で `lambda < mu` の場合は `std::invalid_argument` を送出する。
- 目的関数と遺伝的操作は呼び出し中に有効でなければならず、比較関数は一貫した順序を与えなければならない。
- 結果の再現には、同じ乱数生成器の状態・入力・標準ライブラリの実装が必要である。大域最適性は保証しない。
