---
title: Simple Polygon Triangulation (単純多角形三角形分割)
documentation_of: ../src/algorithm/geometry/2d/simple_polygon_triangulation.hpp
---

単純多角形を三角形分割するための汎用API。

## API

~~~cpp
std::vector<EarClippingTriangle> simple_polygon_triangulation(
    const std::vector<Point>& polygon
);
~~~

polygon の頂点を境界順に渡し、入力配列の頂点添字からなる三角形列を返す。
時計回り・反時計回りのどちらも受理し、返す三角形は反時計回りにそろえる。
現在の実装は `ear_clipping_triangulation` と同じ正規化、単純性検査、
$y$-単調分割、stackによる単調面三角形分割を用いる。

## API別の時間計算量・空間計算量

$N$ を入力頂点数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `simple_polygon_triangulation(polygon)` | $O(N\log N)$ | $O(N)$ |

単純性検査、単調分割、平面埋込みからの面復元を含む。
全ての単調面に対する三角形分割の合計は $O(N)$。

## 注意点

面積が正の単純多角形を対象とする。連続重複頂点、閉路を表す末尾の始点、
辺上の中間頂点は除去される。自己交差、非連続位置の重複頂点、
隣接辺の重なり、または退化した入力には `std::invalid_argument` を
送出する。座標と中間演算は有限な `long double` の範囲に収まる
必要がある。
