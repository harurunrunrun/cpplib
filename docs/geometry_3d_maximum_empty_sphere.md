---
title: Maximum Empty Sphere (最大空球)
documentation_of: ../src/algorithm/geometry/3d/maximum_empty_sphere.hpp
---

有界凸多面体内に中心を制約し、siteを内部に含まない最大半径の球を求める。

## API

- `maximum_empty_sphere(sites, bounds)`: 中心が `bounds` に属する点のうち、最近site
  までの距離を最大化し、その中心と距離を `Sphere3` で返す。内部Voronoi頂点、
  境界面とVoronoi edgeの交点、境界辺とVoronoi ridgeの交点、境界頂点を評価する。

## API別の時間計算量・空間計算量

重複除去後site数を $S$、境界面数を $F$、境界辺数を $E$、Delaunay四面体数を
$T$ として、Delaunay構築を除く時間は $O(TS+ES^3+FS^4)$、追加領域は
$O(S+T+E)$。Delaunay構築の最悪時間は $O(S^3)$。

## 注意点

- 球の中心だけを `bounds` 内に制約する。球全体を `bounds` 内へ収めるAPIではない。
- `bounds` は外向き三角形面を持つ非空の三次元凸多面体でなければならない。
- siteが空、boundsが三次元でない場合は `std::invalid_argument` を送出する。
- 同率最大の中心が複数ある場合、そのうち列挙順で最初のものを返す。
- bounds包含は外向き面の適応的predicateで判定し、辺・面・site差は局所scaleへ
  正規化する。補助交点が `long double` で表現不能な場合はその候補を除外する。
- 最近siteまでの真の距離が `long double` で表現不能な場合は
  `std::overflow_error` を送出する。
