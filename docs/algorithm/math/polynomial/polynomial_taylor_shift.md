---
title: Polynomial Taylor Shift (多項式の Taylor shift) [polynomial_taylor_shift]
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
返す列の長さは入力と等しい。零多項式を空の列で渡した場合は空の列を返す。

- 時間計算量: $O(M(N))$。NTT を用いる場合は $O(N \log N)$
- 空間計算量: $O(N)$

ここで $N$ は係数の個数、$M(N)$ は次数 $N$ 未満の多項式乗算の時間である。

## 注意点

- `polynomial.size() < MOD` が必要であり、満たさない場合は `std::length_error` を送出する。
- 乗算に NTT を使う場合、必要な 2 の冪の変換長が `MOD - 1` を割り切らなければならない。
