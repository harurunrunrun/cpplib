---
title: Polynomial Modular Composition (法多項式上の多項式合成)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_modular_composition.hpp
---

多項式の合成 $f(g(x))$ を、正の次数をもつ多項式 $h(x)$ で割った余りとして求める。

## API

- `math::polynomial_compose_mod<MOD>(outer, inner, modulus)`: $\mathrm{outer}(\mathrm{inner}(x)) \bmod \mathrm{modulus}(x)$ を、次数が $\deg(\mathrm{modulus})$ 未満の係数列で返す。

## API別の時間計算量・空間計算量

$m$ を `outer` の長さ、$d = \deg(\mathrm{modulus})$、次数 $d$ の多項式同士の乗算と剰余にかかる時間を $R(d)$ とする。平方分割（baby-step giant-step）により、時間計算量は $O(\lceil \sqrt{m} \rceil R(d) + m d)$、追加領域は $O(d \sqrt{m})$ である。

## 注意点

- `modulus` は末尾の零係数を除いた後に正の次数でなければならず、定数または零多項式を渡すと `std::invalid_argument` を投げる。最高次係数は可逆である必要がある（`MOD` が素数なら常に満たされる）。
- 返す係数列の末尾の零係数は除去される。`outer` が空なら空の列を返す。
