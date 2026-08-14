---
title: General Polyhedron Closest Point (一般多面体への最近点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/polyhedron_point/polyhedron_closest_point.hpp
---

凸性を仮定しない閉三角形メッシュの閉領域上で、指定点に最も近い点を返す。外点では三角形AABB階層をbest-first探索する。

## API

- `polyhedron_closest_point(polyhedron, point)`: 内部または境界なら入力点自身、外部なら表面上の最近点を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ とする。包含判定は $O(V+F)$、AABB構築は $O(F\log F)$。
- 最近面探索は通常枝刈りされ、最悪時間計算量は $O(F)$。全体の追加空間計算量は $O(V+F)$。

## 注意点

閉じた一貫した向きの非退化三角形メッシュを前提とする。面が空なら `std::invalid_argument` を送出する。
座標を照会点基準で正規化して探索し、巨大座標差による中間overflowを避ける。
