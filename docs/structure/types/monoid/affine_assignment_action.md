---
title: Affine Function Assignment Action (アフィン関数列の一括代入作用)
documentation_of: ../../../../src/structure/types/monoid/affine_assignment_action.hpp
---

`AffineAssignmentAction<Scalar>` は長さ付き作用モノイドで、列・木の経路上の全関数を
同じ `MonoidAffine<Scalar>` へ置き換える。
`S` は合成されたアフィン関数、`T` は `optional<S>`。`nullopt` は更新なし。
`op` は左から右への関数適用、`mapping` は代入関数の長さ回の合成を計算する。
`LazyLinkCutTree` と可変長列の遅延作用付き構造に渡して使える。

## 時間計算量

`op`, `e`, `composition`, `id` は $O(1)$。
`mapping` は区間長を $L$ として $O(\log(L+1))$ 時間、$O(1)$ 空間。
`LazyLinkCutTree` の経路代入・積はこの作用では償却 $O(\log^2(N+1))$。

## 注意点

長さは非負であること。空区間への代入の集約値は恒等写像。
零の乗数を許す。`Scalar` は整数からの構築と加算・乗算を提供し、中間値を表現できる必要がある。
