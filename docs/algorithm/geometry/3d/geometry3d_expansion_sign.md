---
title: Sign of Floating-Point Expansion (浮動小数点展開の符号)
documentation_of: ../../../../src/algorithm/geometry/3d/geometry3d_expansion_sign.hpp
---

正規化された浮動小数点展開の符号を判定します。

## API

- `geometry3d_expansion_sign(expansion)`: 正なら `1`、負なら `-1`、零なら `0` を返します。

## API別の時間計算量・空間計算量

成分数を $N$ とすると最悪時間 $O(N)$、追加空間 $O(1)$ です。

## 注意点

成分が絶対値の小さい順に並ぶ正規化された展開を前提とします。非有限成分には `std::invalid_argument` を送出します。