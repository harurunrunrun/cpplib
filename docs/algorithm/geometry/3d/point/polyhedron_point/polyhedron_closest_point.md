---
title: General Polyhedron Closest Point (一般多面体上の最近点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/polyhedron_point/polyhedron_closest_point.hpp
---

凸性を仮定しない閉じた三角形メッシュについて、それが囲む閉領域上で指定点に最も近い点を返す。外部の点に対しては、三角形の AABB 階層を構築して最良優先探索する。

## API

- `polyhedron_closest_point(polyhedron, point)`: `point` が内部または境界上なら `point` 自身を、外部なら表面上の最近点を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ とする。包含判定は $O(V+F)$、AABB 構築は $O(F \log F)$ である。
- 最近面の探索は通常は枝刈りが効くが、最悪時間計算量は $O(F \log F)$（全ノードを優先度付きキューで処理する場合）である。
- 全体の追加領域は $O(V+F)$。

## 注意点

- 閉じていて向きが一貫した、非退化な三角形メッシュを前提とする。
- 面が空の場合、`point` や頂点が非有限の場合、同じ頂点を重複して使う面がある場合は `std::invalid_argument` を送出する。面の頂点添字が範囲外なら `std::out_of_range` を送出する。
- 座標を照会点基準で正規化してから探索するため、巨大な座標差による中間値のオーバーフローを避けられる。
