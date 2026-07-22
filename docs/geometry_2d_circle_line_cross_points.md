---
title: 2D Circle-Line Cross Points (二次元円と直線の交点) [CGL_7_D]
documentation_of: ../src/algorithm/geometry/2d/point_collection/circle_line_cross_points.hpp
---

## API

- `circle_line_cross_points(circle, line)`: 円周と無限直線の異なる交点を0～2個返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

接点は1個だけ返す。負半径または退化直線には `std::invalid_argument` を送出する。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 接触・包含の距離二乗や4次量は、比較する各項の大きさにGEOMETRY_EPSを掛けた相対許容誤差で判定する。中間値は有限なlong doubleに収まること。
- lineの方向vector長がGEOMETRY_EPS以下なら退化直線としてstd::invalid_argumentを送出する。
