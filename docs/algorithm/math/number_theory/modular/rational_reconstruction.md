---
title: Rational Reconstruction (有理数復元)
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/rational_reconstruction.hpp
---

合同式 $a\equiv n d^{-1}\pmod m$ と分子・分母の絶対値上限から既約有理数 $n/d$ を復元する。

## API

```cpp
optional<pair<long long, long long>> math::rational_reconstruction(
    long long residue,
    long long modulus,
    long long numerator_bound,
    long long denominator_bound
);
```

条件を満たす候補が得られれば正の分母で返し、得られなければ `nullopt` を返す。

## 時間計算量

拡張Euclid法により $O(\log m)$ 時間、$O(1)$ 空間。

## 注意点

法と分母上限は正、分子上限は非負でなければならない。一意性を保証する十分条件は $2ND<m$ である。
