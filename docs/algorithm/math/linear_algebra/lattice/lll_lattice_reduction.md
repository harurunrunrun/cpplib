---
title: LLL Lattice Basis Reduction (LLL格子基底簡約)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/lattice/lll_lattice_reduction.hpp
---

整数格子基底をsize-reducedかつLovász条件を満たす基底へ変換する。

## `LllLatticeReductionResult`

`basis` は簡約後基底、`transformation` は `basis = transformation * input` を満たす整数行列、`swaps` はLovász交換回数。

## `lll_lattice_reduction(basis, delta=0.75)`

基底数を $b$、次元を $d$、size reductionと交換の実行回数を $I$ とすると、この実装はGram–Schmidtを明示的に再計算するため時間計算量 $O(Ib^2d)$、空間計算量 $O(b^2+bd)$。固定した `delta < 1` では $I$ は入力bit長に対して多項式で抑えられる。

## 注意点

基底は一次独立で、全行の次元が等しくなければならない。`delta` は $(1/4,1)$。整数基本変形が `long long` を超える場合は `std::overflow_error`、不正入力には `std::invalid_argument` を送出する。
