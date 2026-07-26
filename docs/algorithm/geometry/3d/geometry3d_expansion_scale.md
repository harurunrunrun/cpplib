---
title: Scale Floating-Point Expansion (浮動小数点展開のスカラー倍)
documentation_of: ../../../../src/algorithm/geometry/3d/geometry3d_expansion_scale.hpp
---

浮動小数点展開を1個のスカラー倍にします。

## API

- `geometry3d_expansion_scale(expansion, scalar)`: 厳密なスカラー倍を表す展開を返します。

## API別の時間計算量・空間計算量

成分数を $N$ とすると最悪時間 $O(N^2)$、返却値と中間展開の空間は生成される成分数に比例します。

## 注意点

展開の成分と `scalar` は有限値でなければなりません。非有限値は `std::invalid_argument`、overflowは `std::overflow_error` になります。