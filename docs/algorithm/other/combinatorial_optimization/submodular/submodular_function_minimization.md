---
title: Submodular Function Minimization (劣モジュラ関数最小化)
documentation_of: ../../../../../src/algorithm/other/combinatorial_optimization/submodular/submodular_function_minimization.hpp
---

value oracleで与えた劣モジュラ集合関数を、minimum-norm pointによる候補生成と
全部分集合の厳密照合を組み合わせて最小化する。

## `SubmodularMinimizationResult<Value>`

`elements` と `value` は厳密な最小集合とoracle値である。`minimum_norm_point` は
base polytope上の候補点、`iterations` はWolfe反復数、`dual_gap` は停止時gap、
`minimum_norm_converged` はその数値反復がgap条件で収束したかを表す。

## `submodular_function_minimization(ground_set_size, oracle, maximum_iterations=10000, tolerance=1e-12)`

`oracle(set)` は長さ $n$ の0/1 vectorを受け取る。active base数を $a$、反復数を
$I$、oracle時間を $T$ とすると時間計算量は
$O(I(nT+n\log n+a^3+a^2n)+2^n(n+T))$、空間計算量は
$O(an+a^2+n)$。最後の全部分集合照合により、数値反復の収束状況によらず厳密値を返す。

## 注意点

oracle値は有限な数値へ変換でき、関数は劣モジュラでなければならない。
minimum-norm計算は `long double` だが、`elements` と `value` の厳密性には影響しない。
停止条件が不正なら `std::invalid_argument`、active setが数値的に特異なら
`std::runtime_error` を送出する。全部分集合を評価するため、大きい $n$ には適さない。
