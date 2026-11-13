---
title: Affine Function Assignment Action (アフィン関数列の一括代入作用)
documentation_of: ../../../../src/structure/types/monoid/affine_assignment_action.hpp
---

`AffineAssignmentAction<Scalar>` は長さ付き作用モノイドで、列や木の経路上の全関数を
同じ `MonoidAffine<Scalar>` に置き換える作用を表す。
`S` は合成されたアフィン関数、`T` は `optional<S>` であり、`nullopt` は更新なしを表す。
`op` は左から右への順に関数を適用した合成、`mapping` は代入する関数を区間長の回数だけ合成した関数（繰り返し二乗法）を計算する。
`LazyLinkCutTree` や、可変長列の遅延作用付き構造に渡して使える。

## 時間計算量

`op`, `e`, `composition`, `id` は $O(1)$ である。
`mapping` は区間長を $L$ として $O(\log(L+1))$ 時間、$O(1)$ 空間である。
`LazyLinkCutTree` の経路への代入・経路の積は、この作用ではならし $O(\log^2(N+1))$ になる。

## 注意点

- 長さは非負であること。空区間への代入の集約値は恒等写像である。
- 乗数 0 を許す。
- `Scalar` は整数からの構築と加算・乗算を提供し、中間値を表現できる必要がある。
