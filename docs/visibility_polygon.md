---
title: Visibility Polygon (可視領域多角形)
documentation_of: ../src/algorithm/geometry/2d/visibility_polygon.hpp
---

単純多角形の内部にある観測点から、遮られずに見える領域の境界を求める。

## API

~~~cpp
vector<Point> visibility_polygon(
    const vector<Point>& polygon,
    const Point& observer
)
~~~

多角形の各頂点方向と、その直前・直後のrayについて最初に当たる境界点を求め、観測点まわりの反時計回り順で返す。入力polygonは時計回り・反時計回りのどちらでもよい。境界は不透明で、最初の交点より奥は遮蔽される。

## 時間計算量

頂点数をNとするとO(N^2)。単純性検査がO(N^2)、高々3N本のrayと全辺の交差がO(N^2)、角度sortがO(N log(N+1))。

## 空間計算量

O(N)。

## 注意点

- polygonは3頂点以上の有限座標からなる、面積が非零の単純多角形であること。退化辺・自己交差があればinvalid_argument。
- observerはpolygonの厳密な内部にある有限座標であること。境界上または外部ならinvalid_argument。
- 頂点rayの直前・直後には、隣接する相異なる頂点方向までの角度gapの1/4以下、かつ1e-7 rad以下の適応offsetを使う。固定offsetで近接eventを飛び越えない。
- rayと辺の平行判定は方向積に比例する丸め誤差を用い、辺を正数倍しても判定を変えない。rayとの共線判定には方向を正規化した絶対距離誤差を用いる。
- 近接する戻り点の併合距離は、絶対誤差 `8 * GEOMETRY_EPS` と、入力座標の大きさ・観測点からの局所距離に応じた機械丸め誤差の和である。多角形を大きく平行移動しても、局所的に異なる頂点を相対EPSだけで併合しない。
- 計算は `long double` による近似計算である。角度差や辺長が機械精度と同程度の入力では、近接する戻り点が併合され得る。
- 戻り点は入力polygonの境界上にあり、遮蔽境界ではreflex頂点からの延長上の交点も含み得る。
