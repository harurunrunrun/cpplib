---
title: Circle-Triangle Intersection Area (円と三角形の共通部分面積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/circle_point_point_point/circle_triangle_intersection_area.hpp
---

円板と三角形の共通部分の面積を求める。

## API

```cpp
long double circle_triangle_intersection_area(
    const Circle& circle,
    const Point& first,
    const Point& second,
    const Point& third
);
```

## 引数

`circle` は半径が非負の円、3 点は三角形の頂点である。

## 戻り値

頂点の向きによらない非負の面積を `long double` で返す。内部では `circle_polygon_intersection_area` に 3 頂点の多角形として渡す。

## 時間計算量

時間計算量・追加領域ともに $O(1)$。

## 注意点

- 退化した三角形では 0 を返す。ただし、微小でも非退化な三角形を絶対許容誤差だけで退化扱いすることはない。
- 円の中心座標と半径は有限値でなければならず、非有限値や負の半径には `std::invalid_argument` を送出する。
- 接触・包含の判定に用いる距離の 2 乗や 4 次の量は、比較する各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で判定する。中間値は有限な `long double` に収まる必要がある。
