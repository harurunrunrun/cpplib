---
title: General Polyhedron Volume (一般多面体の体積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/polyhedron/polyhedron_volume.hpp
---

## API

- `polyhedron_volume(polyhedron)`: 一般多面体の符号付き体積の絶対値を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間計算量 $O(V+F)$、追加空間計算量 $O(1)$。

## 注意点

凸性は要求しない。閉じた一貫した向きの三角形面を前提とし、入力検査と例外は `polyhedron_signed_volume` に従う。
