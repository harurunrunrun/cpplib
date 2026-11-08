---
title: 2D Tangent Points (二次元円への接点) [CGL_7_F]
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/circle_point/tangent_points.hpp
---

## API

- `std::vector<Point> tangent_points(circle, point)`: `point` を通る円の接線について、円周上の相異なる接点を $0$ ～ $2$ 個返す。2 個のときは $(x, y)$ の辞書順に並べる。

## API別の時間計算量・空間計算量

- `tangent_points`: 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

- 点が円の内部にあれば空列、円周上にあればその点 1 個、外部にあれば 2 点を返す。
- 半径がちょうど $0$ の円に対しては、`point` によらず中心 1 点を接点として返す。
- 円の中心座標と半径は有限値でなければならない。非有限値または負の半径には `std::invalid_argument` を送出する。
- 内外の判定では、距離の二乗と半径の二乗を、各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で比較する。中間値は有限な `long double` に収まる必要がある。
