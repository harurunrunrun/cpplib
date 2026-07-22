---
title: Circle-Polygon Intersection Area Function (円と多角形の共通部分面積関数)
documentation_of: ../src/algorithm/geometry/2d/scalar/circle_polygon_intersection_area_value.hpp
---

## API

- `circle_polygon_intersection_area(circle, polygon)`: 円盤と単純多角形の共通部分の非負面積を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(1)$。

## 注意点

多角形は時計回り・反時計回りのどちらでもよい。負半径には `std::invalid_argument` を送出する。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 接触・包含の距離二乗や4次量は、比較する各項の大きさにGEOMETRY_EPSを掛けた相対許容誤差で判定する。中間値は有限なlong doubleに収まること。
