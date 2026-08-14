---
title: General Polyhedron Signed Volume (一般多面体の符号付き体積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/polyhedron/polyhedron_signed_volume.hpp
---

凸性を仮定しない閉三角形メッシュの符号付き体積を、有向四面体の和で計算する。

## API

- `polyhedron_signed_volume(polyhedron)`: 外向き面なら正、逆向き面なら負の体積を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間計算量 $O(V+F)$、追加空間計算量 $O(1)$。

## 注意点

面は閉じ、向きが一貫している必要がある。非有限入力・不正indexを拒否し、有限値で表せない結果には `std::overflow_error` を送出する。
