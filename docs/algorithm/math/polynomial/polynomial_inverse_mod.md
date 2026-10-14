---
title: Polynomial Inverse Modulo Polynomial (多項式を法とする逆元) [inv_of_polynomials]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_inverse_mod.hpp
---

`math::polynomial_inverse_mod<MOD>(f,g)` は `f*h=1 mod g` かつ `deg(h)<deg(g)` の `h` を `optional<Polynomial<MOD>>` で返す。素数体を前提とし、互いに素でなければ `nullopt`。`g=0` は例外、非零定数 `g` の商環では空の多項式を返す。

## 時間計算量

Half-GCD による拡張 Euclid 法を使用する。次数を $N$ として $O(M(N)\log N)$ 時間、再帰中間領域を含め $O(N\log N)$ 領域。

`polynomial_solve_congruence<MOD>(f,b,g)` は `f*h=b mod g` を解く。`f` と `g` が互いに素でなくてもよく、`gcd(f,g)` が `b` を割らなければ `nullopt`。返す代表元の次数は `deg(g/gcd(f,g))` 未満。

類題: 多項式合同式を非互いに素の場合も含めて解く。計算量は逆元と同じ。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
