---
title: 2D Circle-Segment Cross Points (二次元円と線分の交点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/circle_segment/circle_segment_cross_points.hpp
---

## API

- `std::vector<Point> circle_segment_cross_points(circle, segment)`: 円周と閉線分の相異なる交点を $0$ ～ $2$ 個返す。2 個のときは $(x, y)$ の辞書順に並べる。

## API別の時間計算量・空間計算量

- `circle_segment_cross_points`: 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

- 線分の端点での交差も含み、同じ点は重複して返さない。
- 円の中心座標と半径は有限値でなければならない。非有限値または負の半径には `std::invalid_argument` を送出する。
- `segment` の方向ベクトルの長さが `GEOMETRY_EPS` 以下なら、`segment.a` の 1 点として扱う。
- 接触・包含の判定では、距離の二乗などの量を、比較する各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で比較する。中間値は有限な `long double` に収まる必要がある。
