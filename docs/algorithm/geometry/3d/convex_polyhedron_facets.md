---
title: Convex Polyhedron Facets (三次元凸多面体の幾何面)
documentation_of: ../../../../src/algorithm/geometry/3d/convex_polyhedron_facets.hpp
---

## API

- `convex_polyhedron_facets(polyhedron)`: 三角形分割の内部対角線を除去し、幾何学的な面ごとにまとめる。

## API別の時間計算量・空間計算量

- `convex_polyhedron_facets`: 頂点数を $V$、三角形数を $F$ として、
  時間 $O(V+F)$、追加領域 $O(V+F)$。頂点添字をkeyとする2回の
  安定計数sortで無向辺をまとめ、共面三角形の連結成分探索と、
  頂点添字配列による各成分の境界復元を行う。

## 注意点

- 全頂点の座標は有限でなければならず、違反時は
  `std::invalid_argument` を送出する。
- 面添字は範囲内で、同じ幾何面に属する三角形は一貫した向きに並ぶ
  必要がある。
- アフィン次元2では外周に一度だけ現れる辺を受理する。アフィン次元3
  以上では全ての辺がちょうど二つの三角形に属する必要がある。
- 異なる添字が同一直線上にある場合を含む退化三角形、非多様体辺、
  開いた境界には `std::domain_error`、
  範囲外添字には `std::out_of_range` を送出する。
- 戻り値のfacet順、各 `boundary` の始点、`triangles` の順序は
  API契約に含まれない。
- アフィン次元が2未満なら空列を返す。
