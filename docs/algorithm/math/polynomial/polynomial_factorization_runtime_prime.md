---
title: Runtime Prime Polynomial Factorization (実行時の素数法での因数分解) [factorization_of_polynomials]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_factorization_runtime_prime.hpp
---

## API

- `math::RuntimePrimePolynomialFactorizer(prime)(coefficients)`: 実行時に指定した素数 $p$（`prime`）を法とする体で、多項式を monic な既約因子に分解する。
    - 法は $2 \le p \le 2147483647$ の素数でなければならない。
    - 入力 `coefficients` は `std::vector<std::uint32_t>`（係数は定数項から次数の昇順）で、各係数は内部で $p$ を法として正規化する。
    - 結果は、最高次の係数 `leading_coefficient` と、`polynomial` および `multiplicity` を持つ因子の列である。

同じ整数係数の多項式を複数の素数で因数分解し、因子の個数や次数の構成を比較するといった用途を想定する。

## アルゴリズム

平方因子の分離、次数別分解、Cantor–Zassenhaus 法による等次数分解を行う。標数 $2$ の場合や、導関数が $0$ になる場合も扱う。

## 時間計算量・空間計算量

次数を $N$ とする。係数の積・除算は $O(N^2)$ の素朴な方法で行うので、小さい次数向けである。全体の期待時間は $O(N^3 \log p \log(N+2))$、領域は $O(N^2)$。最大次数 100 程度の小さい多項式に適する。次数が大きく、法がコンパイル時の定数なら `polynomial_factorization_finite_field` を使う。

## 注意点

- 法が範囲外、または素数でない（試し割りで判定）場合は、コンストラクタが `std::invalid_argument` を送出する。
- 零多項式（正規化後に空になるもの）を渡すと `std::invalid_argument` を送出する。
- GCC 13 / C++20 を想定する。
