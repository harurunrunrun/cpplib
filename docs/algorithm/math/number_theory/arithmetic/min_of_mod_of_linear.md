---
title: Minimum of a Linear Function Modulo m (一次式の剰余の最小値) [min_of_mod_of_linear]
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/min_of_mod_of_linear.hpp
---

等差数列を法で折り返した列の最小値を求める。

## `min_of_mod_of_linear`

```cpp
long long math::min_of_mod_of_linear(
    long long count,
    long long modulus,
    long long coefficient,
    long long constant
);
```

`count`, `modulus`, `coefficient`, `constant` をそれぞれ $n, m, a, b$ として、次の値を返す。

$$
\min_{0\le x<n}\,(ax+b)\bmod m
$$

`count > 0`, `modulus > 0`, `0 <= coefficient, constant < modulus` を要求し、条件を満たさない場合は `invalid_argument` を送出する。

係数が法の半分より大きいときは添字を反転し、係数を $m - a$ に置き換える。その後、法の倍数を初めて越えた位置だけを次の問題へ写す。次の問題の法は現在の係数以下であり、係数の反転により現在の法の半分以下になる。

## 時間計算量

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `min_of_mod_of_linear` | $O(\log(m+1))$ | $O(1)$ |

## 注意点

- 積と加算の途中計算には 128 bit 整数を使うため、引数が `long long` の範囲内であればオーバーフローしない。
- `coefficient` と `constant` は事前に $[0, m)$ に正規化しておく必要がある（負の値は `invalid_argument`）。
