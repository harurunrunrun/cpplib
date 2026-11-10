---
title: 2D Circle-Line Cross Points (二次元円と直線の交点) [CGL_7_D]
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/circle_line/circle_line_cross_points.hpp
---

## API

- `std::vector<Point> circle_line_cross_points(circle, line)`: 円周と無限直線の相異なる交点を $0$ ～ $2$ 個返す。交点が 2 個のときは $(x, y)$ の辞書順に並べる。

## API別の時間計算量・空間計算量

- `circle_line_cross_points`: 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

- 直線が円に接する場合は接点を 1 個だけ返す。
- 円の中心座標と半径は有限値でなければならない。非有限値または負の半径には `std::invalid_argument` を送出する。
- `line` の方向ベクトルの長さが `GEOMETRY_EPS` 以下なら退化直線とみなし、`std::invalid_argument` を送出する。
- 接触・包含の判定では、距離の二乗などの量を、比較する各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で比較する。中間値は有限な `long double` に収まる必要がある。
