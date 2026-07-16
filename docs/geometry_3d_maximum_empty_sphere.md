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

重複除去後site数を $S$、境界の頂点数・辺数・面数を $V,E,F$、Delaunay四面体数を
$T$、Voronoi ridge数・edge数を $R,A$ とする。

| siteのaffine dimension | Voronoi構築後の候補評価時間 | 追加空間計算量 |
| --- | --- | --- |
| 3 | $O(VS+T(F+S)+ERS+FA(F+S))$ | $O(S+T+R+A+V+E+F)$ |
| 2以下 | $O(VS+T(F+S)+ES^3+FS^3(F+S))$ | $O(S+T+V+E+F)$ |

三次元siteではVoronoi incidenceだけを走査し、全site pair・triple列挙を行わない。
二次元以下のVoronoi incidenceはこのライブラリが保持しないため、正しさを保つ
fallbackとして境界辺では全pair、境界面では全tripleを列挙する。別途、
3D Voronoi構築の時間・空間計算量を要する。

## 注意点

- 球の中心だけを `bounds` 内に制約する。球全体を `bounds` 内へ収めるAPIではない。
- `bounds` は外向き三角形面を持つ非空の三次元凸多面体でなければならない。
- siteが空、boundsが三次元でない場合は `std::invalid_argument` を送出する。
- 同率最大の中心が複数ある場合、そのうち列挙順で最初のものを返す。
- bounds包含は外向き面の適応的predicateで判定し、辺・面・site差は局所scaleへ
  正規化する。補助交点が `long double` で表現不能な場合はその候補を除外する。
- 最近siteまでの真の距離が `long double` で表現不能な場合は
  `std::overflow_error` を送出する。
