---
title: Sign of Floating-Point Expansion (浮動小数点展開の符号)
documentation_of: ../../../../../../src/algorithm/geometry/3d/integer/mixed/geometry3d_expansion_sign.hpp
---

正規化された浮動小数点展開（expansion）の符号を判定する。

## API

- `geometry3d_expansion_sign(expansion)`: 展開が表す値が正なら `1`、負なら `-1`、零なら `0` を返す。

## API別の時間計算量・空間計算量

成分数を $N$ として、時間 $O(N)$、追加領域 $O(1)$。

## 注意点

成分が絶対値の小さい順に並び、互いに重ならない正規化された展開を前提とする。絶対値が最大の非零成分の符号を返す。非有限な成分には `std::invalid_argument` を送出する。
