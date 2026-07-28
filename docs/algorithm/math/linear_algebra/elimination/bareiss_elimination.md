---
title: Bareiss Fraction-Free Elimination (Bareiss分数なし消去法)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/elimination/bareiss_elimination.hpp
---

整数行列を、途中で分数を作らずに消去する。

## `BareissEliminationResult<Integer>`

`upper` は消去後行列、`rank` は階数、`row_swap_sign` は行交換の符号。保持する行列の空間計算量は $O(mn)$。

## `bareiss_elimination(matrix)`

$m\times n$ 行列をfraction-free eliminationする。時間計算量は $O(mn\min(m,n))$ 回の整数演算、追加空間計算量は返却行列を除いて $O(1)$。

## `bareiss_determinant(matrix)`

正方行列の行列式を厳密に返す。時間計算量は $O(n^3)$ 回の整数演算、空間計算量は $O(n^2)$。

## 注意点

除算が数学的に割り切れる整域を `Integer` として使う。固定長整数では中間値が型の範囲を超えないことが前提。非矩形・非正方入力には `std::invalid_argument` を送出する。
