---
title: Convex Polyhedron Facets (三次元凸多面体の幾何面)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron/convex_polyhedron_facets.hpp
---

三角形分割された凸多面体から、同一平面上の三角形をまとめて幾何的な面を復元する。

## API

- `convex_polyhedron_facets(polyhedron)`: 三角形分割の内部対角線を取り除き、幾何的な面ごとにまとめた `std::vector<ConvexPolyhedronFacet3>` を返す。各面は境界の頂点添字列 `boundary` と、その面に属する三角形の添字列 `triangles` を持つ。

## API別の時間計算量・空間計算量

- `convex_polyhedron_facets`: 頂点数を $V$、三角形数を $F$ とすると、時間 $O(V + F)$、追加空間 $O(V + F)$。頂点添字をキーとする 2 回の安定な計数ソートで無向辺をまとめ、共面な三角形の連結成分探索と、頂点添字で引く配列による各成分の境界復元を行う。

## 注意点

- 全頂点の座標は有限でなければならず、違反した場合は `std::invalid_argument` を送出する。
- 面添字は範囲内で、同じ幾何面に属する三角形は一貫した向きで並んでいる必要がある。
- アフィン次元 2 では、外周に一度だけ現れる辺を受理する。アフィン次元 3 以上では、すべての辺がちょうど 2 つの三角形に属する必要がある。
- 退化三角形（相異なる添字の頂点が同一直線上にある場合を含む）、非多様体辺、開いた境界、向きの不整合には `std::domain_error` を、範囲外の添字には `std::out_of_range` を送出する。
- 戻り値の面の順序、各 `boundary` の始点、`triangles` の順序は仕様に含まれない。
- アフィン次元が 2 未満、または面が空なら空列を返す。
