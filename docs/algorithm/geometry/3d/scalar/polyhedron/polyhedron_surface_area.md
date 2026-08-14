---
title: General Polyhedron Surface Area (一般多面体の表面積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/polyhedron/polyhedron_surface_area.hpp
---

凸性を仮定せず、三角形面の面積の総和を返す。

## API

- `polyhedron_surface_area(polyhedron)`: 全三角形面の面積和を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間計算量 $O(V+F)$、追加空間計算量 $O(1)$。

## 注意点

面の重複や内部面も入力どおり加算する。非有限入力・不正indexを拒否し、有限値で表せない結果には `std::overflow_error` を送出する。
