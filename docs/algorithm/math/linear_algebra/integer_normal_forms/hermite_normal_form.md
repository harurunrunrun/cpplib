---
title: Row Hermite Normal Form (行Hermite標準形)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/integer_normal_forms/hermite_normal_form.hpp
---

整数行基本変形だけを用いて $UA=H$ となる行Hermite標準形を構成する。

## `HermiteNormalFormResult<Integer>`

`matrix` が $H$、`left` がunimodular行列 $U$、`pivot_columns` が各非零行のpivot列。

## `row_hermite_normal_form(matrix)`

pivot列の下をEuclid互除法で0にし、上側を $[0,pivot)$ に正規化する。階数を $r$、入力絶対値上限を $M$ とすると、通常 $O(rmn\log M)$ 回程度の整数演算。空間計算量は $O(m^2+mn)$。

## 注意点

`Integer` は符号付き整数と同じ比較・四則演算・剰余を提供する必要がある。固定長整数では中間値overflowに注意する。非矩形入力には `std::invalid_argument` を送出する。

## 時間計算量

The bound is stated in the API section above.
