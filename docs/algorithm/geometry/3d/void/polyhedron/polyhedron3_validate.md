---
title: Polyhedron Structural Validation (多面体の構造検査)
documentation_of: ../../../../../../src/algorithm/geometry/3d/void/polyhedron/polyhedron3_validate.hpp
---

## API

- `geometry3d_validate(polyhedron)`: 全頂点が有限であり、全ての面indexが範囲内かつ同一頂点を重複しないことを検査する。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間計算量 $O(V+F)$、追加空間計算量 $O(1)$。

## 注意点

非有限頂点と面内の重複indexには `std::invalid_argument`、範囲外indexには `std::out_of_range` を送出する。
閉多様体性、面の向き、自己交差までは検査しない。
