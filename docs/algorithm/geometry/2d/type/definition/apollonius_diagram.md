---
title: Apollonius Diagram (アポロニウス図)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/apollonius_diagram.hpp
---

加法重み付き距離 $|x-p_i|-w_i$ の下側包絡を、直線・双曲線 branch・退化 ray の解析的平面グラフとして構築する。

## API

- `ApolloniusCurveKind` は `line`, `hyperbola`, `ray` の3種。
- `ApolloniusCurve` の `kind`, `first_site`, `second_site` は種類と site 対。`origin`, `axis`, `transverse`, `first_radius`, `second_radius`, `branch` は解析的媒介表示を保持する。
- `ApolloniusCurve::point(parameter)` は曲線上の点、`parameter(point)` は対応 parameter を返す。ともに $O(1)$。
- `ApolloniusEdge::no_vertex` は無限端点の番兵値。`curve`, `first_vertex`, `second_vertex`, `first_parameter`, `second_parameter` は carrier、端点、閉 parameter 区間。
- `ApolloniusLocation::no_site` は空図の番兵値。`site`, `weighted_distance` は最小 site と加法重み付き距離。
- `ApolloniusDiagram()` は空図を作り、`ApolloniusDiagram(sites)` は図を構築する。
- `sites()`, `curves()`, `vertices()`, `edges()`, `cell_edges()` は格納列への const 参照を返す。
- `locate(point)` は最小 site と距離を返し、同値なら最小 site 番号を選ぶ。
- `on_edge(edge, point)` は閉 parameter 区間、site 対の等距離、全 site の下側包絡を照合する。

## 時間計算量

site 数を $N$ とする。$O(N^3)$ 個の triple event と curve 区間の下側包絡判定を含め、構築は最悪
$O(N^4)$ 時間、$O(N^3)$ 空間。`locate` と `on_edge` は $O(N)$、member 参照は $O(1)$。

## 注意点

曲線は polyline 近似ではない。有限座標・有限重みが必要。collinear な重み付き triple から生じる2個の頂点も扱う。
同一座標かつ同一重みの site は同じ距離関数になるため、別 cell としての一意性は保証しない。
