---
title: Maximum Empty Sphere (最大空球)
documentation_of: ../src/algorithm/geometry/3d/shape/maximum_empty_sphere.hpp
---

有界凸多面体内に中心を制約し、siteを内部に含まない最大半径の球を求める。

## API

- `maximum_empty_sphere(sites, bounds)`: 中心が `bounds` に属する点のうち、最近site
  までの距離を最大化し、その中心と距離を `Sphere3` で返す。内部Voronoi頂点、
  境界面とVoronoi edgeの交点、境界辺とVoronoi ridgeの交点、境界頂点を評価する。
  三次元siteでは各候補にincidentなDelaunay siteを保持し、その1点までの距離を
  半径として使う。

## API別の時間計算量・空間計算量

重複除去後site数を $S$、境界の頂点数・辺数・面数を $V,E,F$、Delaunay四面体数を
$T$、異なる有限Voronoi頂点数を $U$、Voronoi ridge数・edge数を $R,A$ とする。
$W$ は境界頂点を含むcellへDelaunay隣接上で歩くときのhalfspace判定総数、$P$ は
境界辺とridgeの交点候補が実際にそのridge上かを確認するhalfspace判定総数とする。

| siteのaffine dimension | Voronoi構築後の候補評価時間 | 追加空間計算量 |
| --- | --- | --- |
| 3 | $O(T+W+UF+ER+P+FA+F\log F)$ | $O(S+T+R+A+U+V+E+F)$ |
| 2以下 | $O(VS+ES^3+FS^4+F\log F)$ | $O(S+V+E+F)$ |

三次元siteで列挙・検査される候補数を $C=O(V+U+ER+FA)$ とすると、候補の半径評価
自体は合計 $O(C)$ であり、以前の候補ごとの全site走査 $O(CS)$ は発生しない。
有限Voronoi頂点はincident cell、Voronoi edge/ridge上の候補はそのedge/ridgeを定義する
Delaunay siteから半径を得る。境界頂点だけはVoronoi cell間を「より近い隣接site」へ
歩いて定義siteを特定する。$W$ と $P$ はこの所属確認を上表へ明示したものである。

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
  正規化する。境界面との交点は、その三角形面のbarycentric座標でも所属を確認する。
  補助交点が `long double` で表現不能な場合はその候補を除外する。
- 三次元siteの境界辺候補では、二等分面全体ではなく実際のVoronoi ridgeとの交点だけを
  採用するため、incident cellの全halfspaceを検査する。
- 最近siteまでの真の距離が `long double` で表現不能な場合は
  `std::overflow_error` を送出する。
