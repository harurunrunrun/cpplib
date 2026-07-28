---
title: Dirichlet Convolution (Dirichlet畳み込み)
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/dirichlet_convolution.hpp
---

数論的関数のDirichlet畳み込み
$(f*g)(n)=\sum_{d\mid n}f(d)g(n/d)$
をprefix table上で計算する。

## `dirichlet_convolution(first, second)`

両vectorは添字1から $N$ までを使い、添字0を無視する。時間計算量 $O(N\log N)$、返却vectorを含む空間計算量 $O(N)$。

## `dirichlet_inverse(values)`

`values[1]` が可逆な関数のDirichlet逆を返す。約数対を列挙するため時間計算量 $O(N\sqrt N)$、空間計算量 $O(N)$。

## 注意点

入力vectorの長さは等しく正でなければならない。`Value` は加減乗除を提供し、`values[1]` は可逆であること。不正な長さには `std::invalid_argument` を送出する。
