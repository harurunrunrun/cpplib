---
title: Scale Floating-Point Expansion (浮動小数点展開のスカラー倍)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/scalar/geometry3d_expansion_scale.hpp
---

浮動小数点展開（`Geometry3DExpansion`）を 1 個のスカラーで倍する。

## API

- `geometry3d_expansion_scale(expansion, scalar)`: 厳密なスカラー倍を表す展開を返す。

## API別の時間計算量・空間計算量

成分数を $N$ とすると、各成分の厳密積を順に足し合わせるため最悪時間 $O(N^2)$。戻り値と中間展開の空間は、生成される成分数に比例する。

## 注意点

展開の成分と `scalar` は有限値でなければならない。非有限値には `std::invalid_argument` を、オーバーフローした場合は `std::overflow_error` を送出する。
