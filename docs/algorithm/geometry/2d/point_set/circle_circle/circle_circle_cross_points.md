---
title: 2D Circle-Circle Cross Points (二次元円同士の交点) [CGL_7_E]
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/circle_circle/circle_circle_cross_points.hpp
---

## API

- `std::vector<Point> circle_circle_cross_points(first, second)`: 2 つの円周の相異なる交点を $0$ ～ $2$ 個返す。交点が 2 個のときは $(x, y)$ の辞書順に並べる。

## API別の時間計算量・空間計算量

- `circle_circle_cross_points`: 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

- 2 円が接する場合は接点を 1 個だけ返す。
- 一致する正半径の円同士は交点が無限にあるため `std::domain_error` を送出する。同じ点を中心とする半径 $0$ の円同士は、その点を交点として 1 個返す。
- 円の中心座標と半径は有限値でなければならない。非有限値または負の半径には `std::invalid_argument` を送出する。
- 接触・包含の判定では、距離の二乗や 4 次の量を、比較する各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で比較する。中間値は有限な `long double` に収まる必要がある。
