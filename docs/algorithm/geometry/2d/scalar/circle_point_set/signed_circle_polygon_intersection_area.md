---
title: Signed Circle-Polygon Intersection Area (円と多角形の符号付き共通部分面積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/circle_point_set/signed_circle_polygon_intersection_area.hpp
---

円板と単純多角形の共通部分の符号付き面積を求める。

## API

```cpp
long double signed_circle_polygon_intersection_area(
    const Circle& circle,
    const std::vector<Point>& polygon
);
```

多角形の各辺について、円の中心と辺の両端からなる三角形と円板の共通部分の符号付き面積を足し合わせる。

## 時間計算量

頂点数を $N$ として、時間計算量 $O(N)$、追加領域 $O(1)$。

## 注意点

- 多角形が反時計回りなら正、時計回りなら負の値になる。単純多角形の頂点を周回順に与える。
- 円の中心座標と半径は有限値でなければならず、非有限値や負の半径には `std::invalid_argument` を送出する。
- 接触・包含の判定に用いる距離の 2 乗や 4 次の量は、比較する各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で判定する。中間値は有限な `long double` に収まる必要がある。
- 半径が厳密に 0 の場合と頂点数が 2 未満の場合は、ただちに 0 を返す。
