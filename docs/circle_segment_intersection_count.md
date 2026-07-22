---
title: Circle-Segment Intersection Count (円と線分の交点数)
documentation_of: ../src/algorithm/geometry/2d/scalar/circle_segment_intersection_count.hpp
---

## API

- `circle_segment_intersection_count(circle, segment)`: 円周と閉線分の異なる交点数を返す。

## 引数

`circle` は非負半径の円、`segment` は退化してもよい閉線分である。

## 戻り値

0、1、2のいずれかを返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

端点接触を含む。負半径には `std::invalid_argument` を送出する。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 接触・包含の距離二乗や4次量は、比較する各項の大きさにGEOMETRY_EPSを掛けた相対許容誤差で判定する。中間値は有限なlong doubleに収まること。
- segmentの方向vector長がGEOMETRY_EPS以下ならsegment.aの1点として扱う。
