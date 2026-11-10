---
title: Segment Voronoi Diagram (線分Voronoi図)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/segment_voronoi_diagram.hpp
---

線分の site を両端点と内部（射影が内部に落ちる領域）の要素（feature）に分解し、直線・放物線の二等分線の下側包絡を解析的な曲線のまま構築する。

## API

- `SegmentVoronoiFeatureKind` は `first_endpoint`, `interior`, `second_endpoint` の 3 種類をもつ scoped 列挙型。
- `SegmentVoronoiCurveKind` は `line` または `parabola`。
- `SegmentVoronoiFeature` の `segment`, `kind` は、要素の元の線分番号と種類。
- `SegmentVoronoiCurve` の `kind`, `first_feature`, `second_feature` は、曲線（carrier）の種類と、等距離となる要素の対。`origin`, `tangent`, `normal`, `focus_height`, `implicit` は媒介変数表示と、陰関数表示 $Ax^2 + Bxy + Cy^2 + Dx + Ey + F = 0$ の係数を保持する。
- `SegmentVoronoiCurve::point(parameter)` はパラメータに対応する曲線上の点を、`parameter(point)` は接線方向のパラメータを返す。どちらも $O(1)$。
- `SegmentVoronoiEdge::no_vertex` は無限遠の端点を表す番兵値。`curve`, `first_vertex`, `second_vertex`, `first_parameter`, `second_parameter` は、辺が載る曲線、両端点、閉じたパラメータ区間を表す。
- `SegmentVoronoiLocation::no_site` は空の図に対する番兵値。`segment`, `closest_point`, `distance` は、最も近い線分、その線分上の最近点、距離を表す。
- `SegmentVoronoiDiagram()` は空の図を作り、`explicit SegmentVoronoiDiagram(std::vector<Segment> segments)` は解析的な図を構築する。
- `sites()`, `curves()`, `vertices()`, `edges()`, `cell_edges()` は各格納列への const 参照を返す。辺の有限な端点は必ず `vertices()` のいずれかであり、その点で `on_edge` は真になる。
- `locate(point)` は最も近い線分を返す。距離が等しい場合は番号が最小の線分を選ぶ。
- `on_edge(edge, point)` は、閉じたパラメータ区間、要素の範囲、全 site の下側包絡を照合する。`edge` が範囲外なら `std::out_of_range` を送出する。

## 時間計算量

線分数を $N$ とする。候補となる曲線は $O(N^2)$ 本、site を共有する曲線の対から生じるイベントは $O(N^3)$ 個ある。
各区間の下側包絡の判定を含め、構築は最悪 $O(N^4)$ 時間、$O(N^3)$ 空間。
`locate` と `on_edge` は $O(N)$、メンバの参照は $O(1)$。

## 注意点

- 4 次以下の多項式を、導関数による実根の分離で解く。固定の格子や折れ線による近似は使わない。
- 退化線分は点の site として扱う。
- 幾何的に同じ線分の組や、正の長さで重なる共線な線分の組はセルが一意に定まらないため、`std::invalid_argument` を送出する。座標が非有限な場合も同様である。
- 端点での接触は許す。ただし共有端点が最近点となる 2 次元の同距離領域は、1 次元の `edges()` / `cell_edges()` には列挙しない。このような点でも `locate` は番号が最小の線分を返す。
- 点の引数は有限値でなければならない。
