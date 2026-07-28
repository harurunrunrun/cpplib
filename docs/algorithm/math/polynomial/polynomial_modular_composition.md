---
title: Polynomial Modular Composition (法多項式上の多項式合成)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_modular_composition.hpp
---

多項式合成 $f(g(x))$ を、正次数多項式 $h(x)$ で割った剰余として求める。

## API

- `polynomial_compose_mod<MOD>(outer, inner, modulus)`:
  $outer(inner(x)) \bmod modulus(x)$ を、次数が `modulus` 未満の係数列で返す。

## API別の時間計算量・空間計算量

$m=|outer|$、$d=\deg(modulus)$、次数 $d$ の剰余付き乗算時間を $R(d)$
とする。平方根分割により $O(\lceil\sqrt m\rceil R(d)+md)$ 時間、
$O(d\sqrt m)$ 追加領域。

## 注意点

`modulus` は正次数で、最高次係数が係数環上で可逆でなければならない。
定数または零多項式を渡した場合は `std::invalid_argument`。返却vectorの
末尾の零係数は除去される。
