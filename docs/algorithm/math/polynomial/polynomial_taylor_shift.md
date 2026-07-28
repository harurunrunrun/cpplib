---
title: Polynomial Taylor Shift (多項式のTaylor shift) [polynomial_taylor_shift]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_taylor_shift.hpp
---

## `math::polynomial_taylor_shift`

```cpp
template<int MOD>
math::Polynomial<MOD> math::polynomial_taylor_shift(
    const math::Polynomial<MOD>& polynomial,
    Modint<MOD> shift
)
```

$f(x)$ の係数列 `polynomial` から $f(x+\mathtt{shift})$ の係数列を返す。
返却列の長さは入力と等しく、零多項式を空列で渡した場合は空列を返す。

- 時間計算量: $O(M(N))$。NTTを用いる場合は $O(N\log N)$
- 空間計算量: $O(N)$

ここで $N$ は係数数、$M(N)$ は次数 $N$ 未満の多項式乗算時間。

## 注意点

`polynomial.size() < MOD` が必要である。また、必要な2冪NTT長が
`MOD - 1` を割り切らなければならない。
