---
title: General Polyhedron Centroid (一般多面体の重心)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/polyhedron/polyhedron_centroid.hpp
---

一様密度で満たされた非凸多面体の体積重心を、有向四面体の重み付き和で返す。

## API

- `polyhedron_centroid(polyhedron)`: 閉三角形メッシュが囲む立体の重心を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間計算量 $O(V+F)$、追加空間計算量 $O(V)$。

## 注意点

閉じた一貫した向きの三角形面と非零体積を前提とする。空入力には `std::invalid_argument`、零体積には `std::domain_error`、overflowには `std::overflow_error` を送出する。
