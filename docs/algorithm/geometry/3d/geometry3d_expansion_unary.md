---
title: Unary Expansion Operation (浮動小数点展開の単項演算)
documentation_of: ../../../../src/algorithm/geometry/3d/geometry3d_expansion_unary.hpp
---

浮動小数点展開の符号反転を提供します。

## API

- `geometry3d_expansion_negate(expansion)`: 全成分の符号を反転した展開を返します。空の展開は `{0}` になります。

## API別の時間計算量・空間計算量

成分数を $N$ とすると時間 $O(N)$、返却値を除く追加空間 $O(1)$ です。

## 注意点

非有限成分を含む入力には `std::invalid_argument` を送出します。