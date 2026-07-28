---
title: Smallest Enclosing Annulus (最小包含円環)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/smallest_enclosing_annulus.hpp
---

全点を含む同心2円について、外半径と内半径の差が最小の有限中心円環を求める。

## API

- `smallest_enclosing_annulus(points)` は `EnclosingAnnulusResult` を返す。
- 空列には零円環を返す。異なる点が2個以下なら幅0の円環を返す。

## 時間計算量

点数を $N$ とする。nearest Voronoi、farthest Voronoi、その overlay に加え、入力点と全 pair midpoint を候補として評価する。
farthest Delaunay ear の包含円判定と最大 $O(N^2)$ 個の候補評価を含み、全体は $O(N^3)$ 時間、$O(N)$ 空間。

## 注意点

異なる点が3個以上すべて共線なら幅の下限0は有限中心で達成されないため `std::domain_error`。
それ以外では nearest/farthest Voronoi vertex と両図の overlay vertex が4点支持円環を含む。座標は有限値でなければならない。
