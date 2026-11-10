---
title: Polynomial Roots over Prime Field (素数体上の多項式根) [polynomial_root_finding]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_roots.hpp
---

`math::polynomial_roots<MOD>(f)` は、`math::Polynomial<MOD>` で与えた多項式 $f$ の素数体 $\mathbb{F}_{\mathrm{MOD}}$ 内の相異なる根を、値の昇順で `std::vector<Modint<MOD>>` として返す。非零定数多項式なら空の列を返し、零多項式なら `std::invalid_argument` を送出する。

## 時間計算量

$\gcd(f, x^{\mathrm{MOD}} - x)$ を計算して重根と次数 $2$ 以上の既約因子を除き、残った一次因子の積だけを Cantor–Zassenhaus 法で分割する。$N = \deg f$、多項式乗算の時間を $M(N)$ として、期待 $O(M(N) \log(N+2) (\log \mathrm{MOD} + \log(N+2)))$ 時間である。後者の対数項は各分割での Half-GCD の分も含む。

## 空間計算量

再帰の中間領域を含めて $O(N \log(N+2))$。

## 使用例

多項式 $f$ と $g$（例えば $f$ の平行移動）に共通する根を列挙するには、先に `polynomial_gcd_half(f, g)` を計算し、その結果にこの関数を適用する。

## 注意点

`MOD` は素数でなければならない（`static_assert` で検査する）。
