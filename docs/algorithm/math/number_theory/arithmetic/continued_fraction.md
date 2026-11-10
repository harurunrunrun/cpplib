---
title: Continued Fraction Expansion (連分数展開)
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/continued_fraction.hpp
---

有理数の単純連分数展開と、その各収束分数（近似分数）を求める。

## API

```cpp
vector<long long> math::continued_fraction_expansion(
    long long numerator, long long denominator
);
```

$\mathrm{numerator}/\mathrm{denominator}$ の連分数展開の係数列を、床除算に基づく標準形で返す。末尾の係数が `1` となる二重の表現は、直前の係数に統合する（`[..., a, 1]` を `[..., a+1]` とする）。

```cpp
vector<pair<__int128, __int128>>
math::continued_fraction_convergents(const vector<long long>& coefficients);
```

係数列の各接頭辞が表す分数の（分子, 分母）を返す。

## 時間計算量

係数列の長さを $L$ として、両 API とも $O(L)$ 時間・$O(L)$ の出力空間。展開の $L$ は Euclid の互除法と同じく $O(\log \max(\lvert a\rvert,\lvert b\rvert))$ である。

## 注意点

- 分母が `0` なら `runtime_error` を送出する。
- 末尾の統合で係数が `long long` の最大値を超える場合は `overflow_error` を送出する。
- 収束分数の計算範囲は `__int128` であり、係数列を外部から与える場合はその範囲に収まることを前提とする。
