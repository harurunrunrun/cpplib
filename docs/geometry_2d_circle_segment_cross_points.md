---
title: 2D Circle-Segment Cross Points (二次元円と線分の交点)
documentation_of: ../src/algorithm/geometry/2d/point_collection/circle_segment_cross_points.hpp
---

## API

- `circle_segment_cross_points(circle, segment)`: 円周と閉線分の異なる交点を0～2個返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

端点の交差を含み、重複点は除く。負半径には `std::invalid_argument` を送出し、退化線分は1点として扱う。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 接触・包含の距離二乗や4次量は、比較する各項の大きさにGEOMETRY_EPSを掛けた相対許容誤差で判定する。中間値は有限なlong doubleに収まること。
- segmentの方向vector長がGEOMETRY_EPS以下ならsegment.aの1点として扱う。
