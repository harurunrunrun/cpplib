---
title: Metropolis–Hastings Sampler (Metropolis–Hastingsサンプラー)
documentation_of: ../src/approximate/randomized/metropolis_hastings.hpp
---

非正規化log densityからMetropolis--Hastings法で標本を生成する。

## `MetropolisHastingsResult<State>`

- `samples`: burn-in後、`thinning`遷移ごとに保存した状態。
- `accepted`: burn-inを含む採択回数。
- `proposals`: burn-inを含む提案回数。
- `acceptance_rate()`: `accepted / proposals`。提案がなければ0。

## `metropolis_hastings`

```cpp
auto result = metropolis_hastings(
    initial_state, sample_count, burn_in, thinning,
    log_density, proposal, log_proposal_ratio, random
);
```

`proposal(current, random)`で候補を作る。`log_proposal_ratio(current, candidate)`は
$\log q(current\mid candidate)-\log q(candidate\mid current)$ を返す。
提案1回の時間を$C$、状態の保存領域を$S$とすると、時間
$O((B+KT)C)$、結果を含む追加領域$O(KS)$。ここで$B$は`burn_in`、$K$は
`sample_count`、$T$は`thinning`である。

## `metropolis_hastings_symmetric`

```cpp
auto result = metropolis_hastings_symmetric(
    initial_state, sample_count, burn_in, thinning,
    log_density, proposal, random
);
```

対称提案 $q(y\mid x)=q(x\mid y)$ 用で、proposal ratioを0として上のAPIを実行する。
時間・領域は同じ。

初期状態のlog densityは有限でなければならない。候補では有限値または`-infinity`を
許し、後者は棄却する。`NaN`、`+infinity`、非有限proposal ratioは
`std::domain_error`となる。`sample_count==0`、`thinning==0`は
`std::invalid_argument`、遷移回数の`size_t` overflowは`std::overflow_error`。

目標分布に収束するには、提案kernelが既約かつ非周期的で、記載したproposal ratioが
正しい必要がある。burn-in、thinning、自己相関の程度は呼び出し側で選ぶ。
