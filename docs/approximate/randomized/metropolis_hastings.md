---
title: Metropolis–Hastings Sampler (Metropolis–Hastings サンプラー)
documentation_of: ../../../src/approximate/randomized/metropolis_hastings.hpp
---

正規化されていない対数密度から、Metropolis–Hastings 法で標本を生成する。

## `MetropolisHastingsResult<State>`

- `samples`: burn-in の後、`thinning` 回の遷移ごとに保存した状態。
- `accepted`: burn-in を含む採択回数。
- `proposals`: burn-in を含む提案回数。
- `acceptance_rate()`: `accepted / proposals`。提案が 1 回もなければ 0。

## `metropolis_hastings`

```cpp
auto result = metropolis_hastings(
    initial_state, sample_count, burn_in, thinning,
    log_density, proposal, log_proposal_ratio, random
);
```

`proposal(current, random)` で候補を作る。現在の状態を $x$、候補を $y$ とすると、`log_proposal_ratio(current, candidate)` は $\log q(x \mid y) - \log q(y \mid x)$ を返さなければならない。

$B$ を `burn_in`、$K$ を `sample_count`、$T$ を `thinning`、提案 1 回の時間を $C$、状態 1 つの保存に必要な空間を $S$ とすると、時間計算量は $O((B + KT) C)$、結果を含む追加空間計算量は $O(KS)$。

## `metropolis_hastings_symmetric`

```cpp
auto result = metropolis_hastings_symmetric(
    initial_state, sample_count, burn_in, thinning,
    log_density, proposal, random
);
```

対称な提案 $q(y \mid x) = q(x \mid y)$ 用で、提案比の対数を 0 として上の API を実行する。時間計算量・追加空間計算量は同じ。

## 注意点

- 初期状態の対数密度は有限でなければならない。候補の対数密度は有限値または $-\infty$ を許し、$-\infty$ の候補は棄却する。
- `NaN`、$+\infty$ の対数密度や、有限でない提案比に対しては `std::domain_error` を送出する。
- `sample_count == 0` または `thinning == 0` なら `std::invalid_argument`、遷移回数が `size_t` でオーバーフローするなら `std::overflow_error` を送出する。
- 目標分布へ収束するには、提案カーネルが既約かつ非周期的で、与えた提案比が正しくなければならない。burn-in、間引き、自己相関の程度は呼び出し側で選ぶ。
- 同じ結果を再現するには、同じ乱数生成器の状態・入力・標準ライブラリ実装が必要である。
