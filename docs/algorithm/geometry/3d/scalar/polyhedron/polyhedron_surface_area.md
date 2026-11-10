---
title: General Polyhedron Surface Area (一般多面体の表面積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/polyhedron/polyhedron_surface_area.hpp
---

凸性を仮定せず、三角形面の面積の総和を返す。

## API

- `polyhedron_surface_area(polyhedron)`: 全三角形面の面積の和を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として、時間 $O(V + F)$、追加領域 $O(1)$。

## 注意点

- 重複した面や内部の面も入力どおりに加算する。
- 非有限な頂点や同じ頂点を重複して含む面には `std::invalid_argument` を、範囲外の頂点添字には `std::out_of_range` を送出する。
- 結果が `long double` で表現できない場合は `std::overflow_error` を送出する。
