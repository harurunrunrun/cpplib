---
title: Linear Recurrence Interval Sum (線形漸化式の区間和)
documentation_of: ../../../../src/algorithm/math/sequence/linear_recurrence_sum.hpp
---

`math::linear_recurrence_prefix_sum<MOD>(initial,coefficient,n)` は線形漸化式の最初の `n` 項の和を返す。
`math::linear_recurrence_interval_sum<MOD>(initial,coefficient,l,r)` は `[l,r)` の和を返す。
`initial[i]` は初期項、`coefficient[j]` は `a_i=sum_j coefficient[j]*a_(i-1-j)` の係数。

## 時間計算量

母関数の分母を `Q(x)` から `(1-x)Q(x)` へ変換し、次数を1増やした漸化式へ Bostan--Mori 法を適用する。
次数を `d` として時間 $O(M(d)\log(r+1))$、追加領域 $O(d)$。区間長には比例しない。

## 注意点

GCC 13 / C++20。2入力列の長さは一致させ、`l<=r` を満たす必要がある。添字は `uint64_t`。
空の初期列は零列を表す。使用するNTT長が法で許容されることが必要。

類題: 長さが $10^{18}$ の区間の総和を、連続項を列挙せずに求める。
