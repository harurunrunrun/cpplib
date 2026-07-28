---
title: Smith Normal Form (Smith標準形)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/integer_normal_forms/smith_normal_form.hpp
---

整数行列 $A$ を $UAV=D$ と分解する。$U,V$ はunimodular、$D$ は非負対角成分が順に割り切れる対角行列。

## `SmithNormalFormResult<Integer>`

`diagonal` が $D$、`left` が $U$、`right` が $V$、`rank` が非零対角成分数。

## `smith_normal_form(matrix)`

Euclid互除法型の行・列基本変形でSmith標準形を求める。$r=\min(m,n)$、入力絶対値上限を $M$ としたとき、通常 $O(rmn\log M)$ 回程度の整数演算を行う。整数のbit長増大を含むbit計算量は入力に依存する。空間計算量は $O(m^2+n^2+mn)$。

## 注意点

`Integer` は符号付き整数と同じ比較・四則演算・剰余を提供する必要がある。固定長整数では基本変形の中間値が範囲内であることが前提。非矩形入力には `std::invalid_argument` を送出する。
