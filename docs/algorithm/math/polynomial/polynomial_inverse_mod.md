---
title: Polynomial Inverse Modulo Polynomial (多項式を法とする逆元) [inv_of_polynomials]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_inverse_mod.hpp
---

素数 `MOD` 上で、多項式 $g$ を法とする多項式 $f$ の逆元や、多項式合同式の解を求める。

## API

- `math::polynomial_inverse_mod<MOD>(f, g)`: $f h \equiv 1 \pmod{g}$ かつ $\deg h < \deg g$ を満たす $h$ を `std::optional<Polynomial<MOD>>` で返す。$f$ と $g$ が互いに素でなければ `std::nullopt` を返す。
- `math::polynomial_solve_congruence<MOD>(f, b, g)`: $f h \equiv b \pmod{g}$ の解 $h$ を 1 つ返す。$f$ と $g$ は互いに素でなくてよい。$\gcd(f, g)$ が $b$ を割り切らなければ `std::nullopt` を返す。返す代表元の次数は $\deg(g / \gcd(f, g))$ 未満である。

どちらの関数も、$g$ が零多項式なら `std::invalid_argument` を投げ、$g$ が非零定数なら（商環が自明なので）空の多項式を返す。

## 時間計算量

Half-GCD による拡張 Euclid 互除法を用いる。次数を $N$、多項式乗算の時間を $M(N)$ として、どちらの関数も $O(M(N) \log N)$ 時間である。

## 空間計算量

再帰の中間領域を含めて $O(N \log N)$。

## 注意点

`MOD` は素数を前提とする（体上の多項式環として扱う）。
