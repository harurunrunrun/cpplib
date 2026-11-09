---
title: Circle-Polygon Intersection Area Function (円と多角形の共通部分面積関数)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/circle_point_set/circle_polygon_intersection_area_value.hpp
---

円板と単純多角形の共通部分の面積を求める。

## API

```cpp
long double circle_polygon_intersection_area(
    const Circle& circle,
    const std::vector<Point>& polygon
);
```

共通部分の非負の面積を返す。`signed_circle_polygon_intersection_area` の絶対値である。

## 時間計算量

頂点数を $N$ として、時間計算量 $O(N)$、追加領域 $O(1)$。

## 注意点

- 多角形の頂点の向きは時計回り・反時計回りのどちらでもよい。
- 円の中心座標と半径は有限値でなければならず、非有限値や負の半径には `std::invalid_argument` を送出する。
- 接触・包含の判定に用いる距離の 2 乗や 4 次の量は、比較する各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で判定する。中間値は有限な `long double` に収まる必要がある。
