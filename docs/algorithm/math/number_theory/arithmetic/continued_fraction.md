---
title: Continued Fraction Expansion (連分数展開)
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/continued_fraction.hpp
---

有理数の単純連分数展開と、その各収束分数を求める。

## API

```cpp
vector<long long> math::continued_fraction_expansion(
    long long numerator, long long denominator
);
```

床除算に基づく標準形を返す。末尾係数が `1` となる二重表現は直前へ統合する。

```cpp
vector<pair<__int128, __int128>>
math::continued_fraction_convergents(const vector<long long>& coefficients);
```

各接頭辞が表す分子・分母を返す。

## 時間計算量

係数列長を $L$ として、両APIとも $O(L)$ 時間・$O(L)$ 出力空間。展開の $L$ はEuclid法と同じ $O(\log \max(|a|,|b|))$。

## 注意点

分母 `0` では `runtime_error` を送出する。収束分数の演算範囲は `__int128` であり、係数列を外部から与える場合はその範囲内であることを前提とする。
