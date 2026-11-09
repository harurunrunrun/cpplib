---
title: Smallest Enclosing Annulus (最小包含円環)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/smallest_enclosing_annulus.hpp
---

全点を含む同心円の組（円環）のうち、外半径と内半径の差が最小で、中心が有限なものを求める。

## API

```cpp
EnclosingAnnulusResult smallest_enclosing_annulus(
    const std::vector<Point>& points
);
```

- 返り値は中心 `center`、内半径 `inner_radius`、外半径 `outer_radius` を持つ。
- 空列に対しては中心・半径がすべて 0 の円環を返す。相異なる点が 2 個以下なら幅 0 の円環を返す。

## 時間計算量

点数を $N$ とする。最近点 Voronoi 図の頂点、最遠点 Voronoi 図の頂点、両図の辺どうしの交点（overlay の頂点）に加え、各入力点と全点対の中点を中心の候補として評価する。最遠点 Delaunay 三角形分割の ear に対する包含円判定と、最大 $O(N^2)$ 個の候補の評価（1 候補あたり $O(N)$）を含み、全体で時間計算量 $O(N^3)$、空間計算量 $O(N)$ である。

## 注意点

- 相異なる点が 3 個以上あってすべて共線な場合、幅の下限 0 は有限な中心では達成されないため `std::domain_error` を送出する。
- それ以外の場合は、最近点・最遠点 Voronoi 図の頂点と両図の overlay の頂点の中に、4 点で支持される最適な円環の中心が含まれる。
- 座標は有限値でなければならず、そうでなければ `std::invalid_argument` を送出する。
