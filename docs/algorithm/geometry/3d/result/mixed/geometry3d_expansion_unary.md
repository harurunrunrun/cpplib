---
title: Unary Expansion Operation (浮動小数点展開の単項演算)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/mixed/geometry3d_expansion_unary.hpp
---

浮動小数点展開（`Geometry3DExpansion`）の符号反転を提供する。

## API

- `geometry3d_expansion_negate(expansion)`: 全成分の符号を反転した展開を返す。空の展開に対しては `{0}` を返す。

## API別の時間計算量・空間計算量

成分数を $N$ とすると、時間 $O(N)$。引数を値で受け取ってインプレースで反転するため、戻り値を除く追加空間は $O(1)$。

## 注意点

非有限な成分を含む入力には `std::invalid_argument` を送出する。
