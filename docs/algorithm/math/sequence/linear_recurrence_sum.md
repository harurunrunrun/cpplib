---
title: Linear Recurrence Interval Sum (線形漸化式の区間和)
documentation_of: ../../../../src/algorithm/math/sequence/linear_recurrence_sum.hpp
---

`initial[i]` $= a_i$ を初期項、`coefficient[j]` を係数とする線形漸化式

$$
a_i=\sum_{j=0}^{d-1}\mathtt{coefficient}_j\,a_{i-1-j}
$$

について、項の和を求める。

## API

- `math::linear_recurrence_prefix_sum<MOD>(initial, coefficient, n)`: 先頭 $n$ 項の和 $\sum_{i<n} a_i$ を返す。
- `math::linear_recurrence_interval_sum<MOD>(initial, coefficient, l, r)`: 半開区間 $[l, r)$ の和 $\sum_{l \le i < r} a_i$ を返す。

添字は `std::uint64_t` である。

## 時間計算量

母関数の分母を $Q(x)$ から $(1-x)Q(x)$ に変換し、次数を $1$ 増やした漸化式に Bostan–Mori 法（`bostan_mori`）を適用する。
次数を $d$、多項式乗算の時間を $M(d)$ として、時間 $O(M(d)\log(r+1))$ である。区間の長さには比例しない。

## 空間計算量

$O(d)$。

## 使用例

長さが $10^{18}$ の区間の総和を、連続項を列挙せずに求められる。

## 注意点

- 2 つの入力列の長さは一致させる必要があり、異なる場合は `std::invalid_argument` を送出する。
- `l > r` なら `std::invalid_argument` を送出する。
- 空の初期列は零列を表し、和は $0$ になる。
- 乗算には `arbitrary_mod_convolution` を使うため、畳み込みの長さが NTT の変換長の上限以下である必要がある。
