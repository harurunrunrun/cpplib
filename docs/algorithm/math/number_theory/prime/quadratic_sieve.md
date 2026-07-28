---
title: Quadratic Sieve Factorization (二次篩因数分解)
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/quadratic_sieve.hpp
---

$x^2-n$ がfactor base上でsmoothになる関係を集め、$\mathbb F_2$ 線形従属から非自明因数を得る。

## `QuadraticSieveResult`

`factor` は見つかった因数、`tested_values` は試した $x$ の数、`smooth_relations` はsmooth関係数、`factor_base_size` はfactor baseの大きさ。素数入力では `factor == n`。

## `quadratic_sieve_factor(value, factor_base_bound=1000, maximum_tested_values=1000000, extra_relations=16)`

`factor_base_bound` 以下の素数をfactor base候補とし、$\lceil\sqrt n\rceil$ から順に関係を収集する。factor base数を $B$、試行数を $T$ とすると、smoothness判定は $O(TB)$、逐次GF(2)消去は $O(B^3/64)$、空間計算量は $O(B^2/64+B^2)$。64-bit modular multiplicationには `unsigned __int128` を用いる。

## 注意点

確率的・資源上限制約付きの因数探索なので、上限内に十分なsmooth関係が得られない場合は `factor == nullopt`。`value < 2`、0の試行上限などには `std::invalid_argument` を送出する。
