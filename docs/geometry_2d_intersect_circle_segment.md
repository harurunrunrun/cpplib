---
title: 2D Circle-Segment Intersection Test (二次元円と線分の交差判定)
documentation_of: ../src/algorithm/geometry/2d/predicate/intersect_circle_segment.hpp
---

## API

- `intersect_circle_segment(circle, segment)`: 円周と閉線分が共通点を持つか判定する。

## API別の時間計算量・空間計算量

- `intersect_circle_segment`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

接触と端点を含む。負半径には `std::invalid_argument` を送出し、退化線分は1点として扱う。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 接触・包含の距離二乗や4次量は、比較する各項の大きさにGEOMETRY_EPSを掛けた相対許容誤差で判定する。中間値は有限なlong doubleに収まること。
- segmentの方向vector長がGEOMETRY_EPS以下ならsegment.aの1点として、最小・最大距離二乗の同じ規約で判定する。
