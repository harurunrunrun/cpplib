---
title: Apollonius Diagram (アポロニウス図)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/apollonius_diagram.hpp
---

加法重み付き距離 $\lVert x - p_i \rVert - w_i$ の下側包絡を、直線・双曲線の枝・退化した半直線からなる解析的な平面グラフとして構築する。

## API

- `ApolloniusCurveKind` は `line`, `hyperbola`, `ray` の 3 種類をもつ scoped 列挙型。
- `ApolloniusCurve` の `kind`, `first_site`, `second_site` は曲線の種類と site の対を表す。`origin`, `axis`, `transverse`, `first_radius`, `second_radius`, `branch` は解析的な媒介変数表示を保持する。
- `ApolloniusCurve::point(parameter)` はパラメータに対応する曲線上の点を、`parameter(point)` は点に対応するパラメータを返す。どちらも $O(1)$。
- `ApolloniusEdge::no_vertex` は無限遠の端点を表す番兵値。`curve`, `first_vertex`, `second_vertex`, `first_parameter`, `second_parameter` は、辺が載る曲線（carrier）、両端点、閉じたパラメータ区間を表す。
- `ApolloniusLocation::no_site` は空の図に対する番兵値。`site`, `weighted_distance` は最小の site とその加法重み付き距離を表す。
- `ApolloniusDiagram()` は空の図を作り、`ApolloniusDiagram(sites)`（`std::vector<WeightedPoint>` を受け取る `explicit` コンストラクタ）は図を構築する。
- `sites()`, `curves()`, `vertices()`, `edges()`, `cell_edges()` は格納している列への const 参照を返す。
- `locate(point)` は加法重み付き距離が最小の site とその距離を返す。同値なら番号が最小の site を選ぶ。
- `on_edge(edge, point)` は、点が閉じたパラメータ区間内にあること、site 対から等距離であること、全 site の下側包絡上にあることを照合する。`edge` が範囲外なら `std::out_of_range` を送出する。

## 時間計算量

site 数を $N$ とする。$O(N^3)$ 個の 3 つ組イベントの検査と、曲線上の各区間に対する下側包絡の判定を含め、構築は最悪
$O(N^4)$ 時間、$O(N^3)$ 空間。`locate` と `on_edge` は $O(N)$、メンバの参照は $O(1)$。

## 注意点

- 曲線は折れ線で近似しない。座標と重みは有限でなければならず、そうでなければ `std::invalid_argument` を送出する。
- 重み付きの 3 site が一直線上にある場合に生じる 2 個の頂点も扱う。
- 座標も重みも同じ site は同じ距離関数をもつため、別々のセルとしての一意性は保証しない。座標が同じで重みが異なる site の対や、一方の重み付き円が他方を含む site の対には境界曲線を作らない。
