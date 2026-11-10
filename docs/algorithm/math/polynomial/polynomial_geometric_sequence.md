---
title: Polynomial Geometric Samples (等比点での多項式計算) [multipoint_evaluation_on_geometric_sequence] [polynomial_interpolation_on_geometric_sequence] [shift_of_sampling_points_of_polynomial]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_geometric_sequence.hpp
---

素数 `MOD` を法とする `math::Polynomial<MOD>` を扱う。NTT の長さが法で許容されることが必要である。

## API

$n$ を入力の長さ（多項式の係数の個数または標本の個数）、$m$ を出力の個数、$M(n)$ を多項式の乗算の時間とする。

| 関数 | 結果 | 時間計算量 |
| --- | --- | --- |
| `polynomial_evaluate_geometric(f, a, r, m)` | $f(a r^i)$（$0 \le i < m$） | $O(M(n+m))$ |
| `polynomial_interpolate_geometric(values, a, r)` | 点 $a r^i$ で値 `values[i]` をとる、次数 $n$ 未満の多項式 | $O(M(n))$ |
| `polynomial_shift_samples(values, c, m)` | $f(0), \ldots, f(n-1)$ から $f(c), \ldots, f(c+m-1)$ を求める | $O(M(n+m))$ |

いずれも追加領域は $O(n+m)$。

等比数列上での評価は chirp 変換、等比数列上での補間は点の積の多項式の係数と Lagrange の重みを利用する。$a = 0$、$r = 0$、$r^n = 1$ などの境界のうち有効なものも処理する。標本点のシフトは、法をまたぐ場合や既知の標本点に重なる場合も処理する。区間 $[L, R)$ の連続した標本を別の区間へ移す用途に使える。

## 注意点

- 補間点は相異なる必要があり、$n < \mathrm{MOD}$ でなければならない。補間で $a = 0$、$r = 1$、$r^i = 1$（$0 < i < n$）など点が重なる場合や、$r = 0$ で $n \ne 2$ の場合は `std::invalid_argument` を送出する。
- `polynomial_shift_samples` は $n \ge \mathrm{MOD}$ の場合に `std::invalid_argument` を送出する。
- GCC 13 / C++20 を想定する。
