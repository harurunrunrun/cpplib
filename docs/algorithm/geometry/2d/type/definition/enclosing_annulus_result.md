---
title: Enclosing Annulus Result (包含円環結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/enclosing_annulus_result.hpp
---

包含円環の `center`, `inner_radius`, `outer_radius` を格納する。

## API

`EnclosingAnnulusResult` の3つの公開memberを直接参照する。

## 時間計算量

すべてのmember参照は $O(1)$、空間は $O(1)$。

## 注意点

半径は非負で、内半径以下の点を穴に含めない円環を表す。
