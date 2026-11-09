---
title: Circle-Segment Intersection Count (円と線分の交点数)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/circle_segment/circle_segment_intersection_count.hpp
---

## API

- `int circle_segment_intersection_count(circle, segment)`: 円周と閉線分の相異なる交点の個数を返す。

## 引数

`circle` は非負の半径を持つ円、`segment` は閉線分である（退化していてもよい）。

## 戻り値

0、1、2 のいずれか。端点での接触も数える。内部では `circle_segment_cross_points(circle, segment).size()` を返す。

## API別の時間計算量・空間計算量

時間・追加領域とも $O(1)$。

## 注意点

- 円の中心座標と半径は有限値であること。非有限値または負の半径には `std::invalid_argument` を送出する。
- 接触の判定は、比較する各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で行う。中間値は有限な `long double` に収まる必要がある。
- `segment` の方向ベクトルの長さが `GEOMETRY_EPS` 以下なら、`segment.a` の 1 点として扱う。
