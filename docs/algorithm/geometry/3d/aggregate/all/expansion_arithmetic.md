---
title: Floating-Point Expansion Arithmetic (浮動小数点展開算術)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/expansion_arithmetic.hpp
---

浮動小数点展開（floating-point expansion）の型と、すべての演算の個別ヘッダをまとめて include する、互換のための集約ヘッダである。必要な API だけを使う場合は、対応する個別ヘッダを直接 include すればよい。

## API

- `Geometry3DExpansion`: 展開の成分列。
- `geometry3d_two_sum`, `geometry3d_two_diff`, `geometry3d_two_product`: 2 つのスカラーの和・差・積を誤差なしの展開として返す。
- `geometry3d_expansion_sum`, `geometry3d_expansion_product`: 2 つの展開の和・積。
- `geometry3d_expansion_negate`, `geometry3d_expansion_scale`: 符号反転とスカラー倍。
- `geometry3d_expansion_estimate`, `geometry3d_expansion_sign`: 近似値と符号を返す。

## API 別の時間計算量・空間計算量

成分数を $N, M$ とすると、2 つのスカラーに対する演算は $O(1)$、`negate`・`estimate`・`sign` は $O(N)$、`sum` は $O(M(N+M))$、`scale` は $O(N^2)$、`product` は最悪 $O(N^2 M^2)$ である。戻り値の展開を除く空間は、生成される途中の展開の大きさに比例する。

## 注意点

入力は、有限値であり、かつ絶対値の小さい成分から順に並んだ展開であることを前提とする。
