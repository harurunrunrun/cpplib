---
title: 2D Tangent Points (二次元円への接点) [CGL_7_F]
documentation_of: ../src/algorithm/geometry/2d/point_collection/tangent_points.hpp
---

## API

- `tangent_points(circle, point)`: `point` を通る円の接線について、円周上の異なる接点を0～2個返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

点が円内なら空、円周上なら1点、外部なら2点。負半径には `std::invalid_argument` を送出する。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 接触・包含の距離二乗や4次量は、比較する各項の大きさにGEOMETRY_EPSを掛けた相対許容誤差で判定する。中間値は有限なlong doubleに収まること。
- 半径が厳密に0の円は中心1点を接点として返す。
