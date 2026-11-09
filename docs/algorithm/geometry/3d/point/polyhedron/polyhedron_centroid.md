---
title: General Polyhedron Centroid (一般多面体の重心)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/polyhedron/polyhedron_centroid.hpp
---

一様密度で満たされた非凸多面体の体積重心を、有向四面体の重み付き和で求める。

## API

- `polyhedron_centroid(polyhedron)`: 閉じた三角形メッシュが囲む立体の重心を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F)$、追加領域 $O(V)$。

## 注意点

- 閉じていて向きが一貫した三角形面と、非零の体積を前提とする。
- 非有限の頂点、同じ頂点を重複して使う面、空の頂点集合には `std::invalid_argument` を送出する。
- 面の頂点添字が範囲外なら `std::out_of_range` を送出する。
- 体積の合計が $0$ の場合（全頂点が原点にある場合を含む）は `std::domain_error` を送出する。
- 結果がオーバーフローした場合は `std::overflow_error` を送出する。
