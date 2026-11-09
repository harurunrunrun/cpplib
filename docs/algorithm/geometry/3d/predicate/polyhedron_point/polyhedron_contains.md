---
title: General Polyhedron Point Containment (一般多面体の点包含)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_point/polyhedron_contains.hpp
---

凸性を仮定しない閉三角形メッシュについて、点が内部または境界上にあるかを一般化立体角で判定する。

## API

- `polyhedron_contains(polyhedron, point)`: 点 `point` が境界を含む立体の内部にあれば `true` を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F)$、追加空間 $O(1)$。

## 注意点

- 面は非退化で、全体として閉じた一貫した向きの三角形メッシュをなす必要がある。凸性は要求しない。
- 頂点または `point` が非有限な場合と、同じ頂点を重複して使う面がある場合は `std::invalid_argument`、面の頂点インデックスが範囲外なら `std::out_of_range` を送出する。メッシュが閉じていることや向きの一貫性は検査しない。
- いずれかの面との距離が `GEOMETRY3D_EPS` 以下なら境界上とみなして `true` を返す。
- 面がなければ `false` を返す。
- 立体角の総和の絶対値が $2\pi$ を超えるときに内部と判定するため、面の向きは外向き・内向きのどちらに揃っていてもよい。
