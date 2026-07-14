---
title: Signed Circle-Polygon Intersection Area (円と多角形の符号付き共通部分面積)
documentation_of: ../src/algorithm/geometry/2d/circle_polygon_intersection_area/signed_circle_polygon_intersection_area.hpp
---

## API

- `signed_circle_polygon_intersection_area(circle, polygon)`: 円盤との共通部分の符号付き面積を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(1)$。

## 注意点

多角形が反時計回りなら正、時計回りなら負となる。単純多角形を頂点順に与え、円の半径は非負でなければならない。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 接触・包含の距離二乗や4次量は、比較する各項の大きさにGEOMETRY_EPSを掛けた相対許容誤差で判定する。中間値は有限なlong doubleに収まること。
- 半径が厳密に0の場合だけ面積0へ早期returnする。
