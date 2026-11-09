---
title: Rational Reconstruction (有理数復元)
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/rational_reconstruction.hpp
---

剰余 $a \equiv n d^{-1} \pmod m$ と、分子・分母の絶対値の上限から既約分数 $n/d$ を復元する。

## API

```cpp
std::optional<std::pair<long long, long long>> math::rational_reconstruction(
    long long residue,
    long long modulus,
    long long numerator_bound,
    long long denominator_bound
);
```

`residue` $= a$、`modulus` $= m$、`numerator_bound` $= N$、`denominator_bound` $= D$ とする。$\lvert n \rvert \le N$、$1 \le d \le D$、$\gcd(n, d) = 1$、$a d \equiv n \pmod m$ を満たす分数が拡張ユークリッド互除法で見つかれば `{n, d}`（分母は正）を返し、見つからなければ `std::nullopt` を返す。`residue` は負でもよく、内部で $[0, m)$ に正規化する。

## 時間計算量・空間計算量

拡張ユークリッド互除法により時間 $O(\log m)$、空間 $O(1)$。

## 注意点

- $m > 0$、$D > 0$、$N \ge 0$ でなければならず、違反時は `std::runtime_error` を送出する。
- 解が一意であるための十分条件は $2ND < m$ である。
