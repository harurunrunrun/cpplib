---
title: Runtime Prime Polynomial Factorization (実行時の素数法での因数分解) [factorization_of_polynomials]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_factorization_runtime_prime.hpp
---

`math::RuntimePrimePolynomialFactorizer(prime)(coefficients)` は実行時に指定した素数体で多項式を monic 既約因子へ分解する。法は `2<=prime<=2147483647`。入力は `vector<uint32_t>`。結果は `leading_coefficient` と、`polynomial` および `multiplicity` を持つ因子列。

## 時間計算量

平方因子分離、異次数分解、Cantor--Zassenhaus による同次数分解を行う。標数2と導関数が0になる場合も扱う。係数の積・除算は $O(N^2)$ で、小次数向け。全体の期待時間は $O(N^3\log p\log(N+2))$、領域 $O(N^2)$。最大次数100程度の小さい多項式に適する。大きい次数で法がコンパイル時定数なら既存の `polynomial_factorization_finite_field` を使う。

類題: 同じ整数係数多項式を複数の素数で因数分解し、因子の個数や次数構成を比較する。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
