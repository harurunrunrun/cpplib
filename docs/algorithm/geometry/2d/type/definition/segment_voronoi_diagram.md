---
title: Segment Voronoi Diagram (線分Voronoi図)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/segment_voronoi_diagram.hpp
---

線分 site を端点・内部射影 feature に分解し、直線・放物線 bisector の下側包絡を解析曲線のまま構築する。

## API

- `SegmentVoronoiFeatureKind` は `first_endpoint`, `interior`, `second_endpoint` の3種。
- `SegmentVoronoiCurveKind` は `line` または `parabola`。
- `SegmentVoronoiFeature::segment`, `kind` は feature の元線分番号と種類。
- `SegmentVoronoiCurve` の `kind`, `first_feature`, `second_feature` は carrier の種類と等距離 feature 対。`origin`, `tangent`, `normal`, `focus_height`, `implicit` は媒介表示と二次式係数を保持する。
- `SegmentVoronoiCurve::point(parameter)` は曲線上の点、`parameter(point)` は接線方向 parameter を返す。ともに $O(1)$。
- `SegmentVoronoiEdge::no_vertex` は無限端点を表す番兵値。`curve`, `first_vertex`, `second_vertex`, `first_parameter`, `second_parameter` は carrier、端点、閉 parameter 区間を表す。
- `SegmentVoronoiLocation::no_site` は空図の番兵値。`segment`, `closest_point`, `distance` は最近傍線分、その線分上の最近点、距離。
- `SegmentVoronoiDiagram()` は空図を作り、`SegmentVoronoiDiagram(segments)` は解析的な図を構築する。
- `sites()`, `curves()`, `vertices()`, `edges()`, `cell_edges()` は各格納列への const 参照を返す。有限 edge endpoint は必ず対応する `vertices()` 上にあり `on_edge` が真になる。
- `locate(point)` は最近傍線分を返し、同距離なら最小番号を選ぶ。
- `on_edge(edge, point)` は閉 parameter 区間、feature 範囲、全 site の下側包絡を照合する。

## 時間計算量

線分数を $N$ とする。候補 curve は $O(N^2)$、共有 site を持つ curve 対の event は $O(N^3)$。
各区間の下側包絡判定を含む構築は最悪 $O(N^4)$ 時間、$O(N^3)$ 空間。
`locate` と `on_edge` は $O(N)$、member 参照は $O(1)$。

## 注意点

4次以下の多項式を導関数による実根分離で解き、固定 grid や polyline 近似を使わない。
退化線分は点 site として扱う。幾何的に同じ線分、または正の長さで重なる共線線分は cell が一意でないため `invalid_argument`。端点接触は許すが、共有端点が最近点となる2次元 tie 領域は1次元の `edges()` / `cell_edges()` には列挙しない。`locate` はこの場合も最小線分番号を返す。点引数は有限値でなければならない。
