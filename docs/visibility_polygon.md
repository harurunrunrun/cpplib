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

可視領域の頂点を観測点まわりの反時計回り順で返す。入力polygonは時計回り・反時計回りのどちらでもよい。境界は不透明で、同じ方向では観測点に最も近い境界までを可視とする。遮蔽が切り替わるrayでは、同じray上の近い点と遠い点の両方が結果に現れることがある。

各辺が交わる角度区間を頂点方向間のcellへ登録し、単純多角形の非交差性を利用したoffline lower envelopeから各event直前・直後の最前辺を求める。頂点方向そのものは、そのeventに接続する辺と直前・直後の最前辺だけを比較する。入力に依存する微小角offsetは用いない。

## 時間計算量

頂点数をN、戻り値の点数をKとする。

- 単純性検査: O(N log(N + 1))。Bentley--Ottmann sweepを使い、隣接辺のN交点に加えて1件見つけた時点で打ち切る。
- 角度eventの構築と可視境界計算: O(N log(N + 1) + K)。
- 全体: O(N log(N + 1) + K)。

現APIは非単純多角形を検出するため、入力が単純であることを呼び出し側が既に保証していても検査を省略しない。

## 空間計算量

O(N + K)。

## 注意点

- polygonは3頂点以上の有限座標からなる、面積が非零の単純多角形であること。退化辺・自己交差・隣接辺の重なりがあればinvalid_argument。
- observerはpolygonの厳密な内部にある有限座標であること。境界上または外部ならinvalid_argument。
- 同じray上に複数の頂点があっても一つの角度eventとして処理する。観測点から放射状に伸びる辺はevent上だけで比較し、隣接する開角度区間のactive edgeには入れない。
- 返す点は入力polygonの境界上にある。遮蔽境界ではreflex頂点から延ばしたrayと別の辺との交点も含み得る。
- 計算はlong doubleによる近似計算である。方向差・辺長・交点間距離がGEOMETRY_EPSまたは機械精度と同程度なら、同方向eventや同一点として併合され得る。
- 連続する戻り点の併合距離は、絶対誤差8 * GEOMETRY_EPSと、入力座標の大きさ・観測点からの局所距離に応じた機械丸め誤差の和である。
