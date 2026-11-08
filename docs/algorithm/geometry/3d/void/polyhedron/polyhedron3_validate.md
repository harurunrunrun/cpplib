---
title: Polyhedron Structural Validation (多面体の構造検査)
documentation_of: ../../../../../../src/algorithm/geometry/3d/void/polyhedron/polyhedron3_validate.hpp
---

## API

- `geometry3d_validate(polyhedron)`: 全頂点が有限であること、各面の頂点インデックスがすべて範囲内であり、1 つの面の中で同じ頂点が重複しないことを検査する。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として、時間計算量 $O(V + F)$、追加空間計算量 $O(1)$。

## 注意点

有限でない頂点と面内での頂点インデックスの重複には `std::invalid_argument` を、範囲外のインデックスには `std::out_of_range` を送出する。
閉じた多様体であるか、面の向き、自己交差までは検査しない。
