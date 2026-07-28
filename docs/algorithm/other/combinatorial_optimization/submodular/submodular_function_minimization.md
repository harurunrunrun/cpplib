---
title: Submodular Function Minimization (劣モジュラ関数最小化)
documentation_of: ../../../../../src/algorithm/other/combinatorial_optimization/submodular/submodular_function_minimization.hpp
---

value oracleで与えた劣モジュラ集合関数を、base polytopeのminimum-norm pointとFujishigeの特徴付けを用いて最小化する。

## `SubmodularMinimizationResult<Value>`

`elements` と `value` が最小候補集合とoracle値、`minimum_norm_point` がbase polytope上の点、`iterations` がWolfe反復数、`dual_gap` が停止時gap。

## `submodular_function_minimization(ground_set_size, oracle, maximum_iterations=10000, tolerance=1e-12)`

`oracle(set)` は長さ $n$ の0/1 vectorを受け取る。active base数を $a$、反復数を $I$、oracle時間を $T$ とすると時間計算量 $O(I(nT+n\log n+a^3+a^2n))$、空間計算量 $O(an+a^2)$。最後にminimum-norm pointの座標順prefixをexactな `Value` で評価して返す。

## 注意点

oracle値は有限な数値へ変換でき、関数は劣モジュラでなければならない。minimum-norm計算は `long double` なので、値のscaleに合わせて `tolerance` を設定する。停止条件が不正なら `std::invalid_argument`、active setが数値的に特異なら `std::runtime_error` を送出する。
