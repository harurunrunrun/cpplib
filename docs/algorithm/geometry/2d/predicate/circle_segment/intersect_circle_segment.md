---
title: 2D Circle-Segment Intersection Test (二次元円と線分の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/circle_segment/intersect_circle_segment.hpp
---

## API

- `bool intersect_circle_segment(circle, segment)`: 円周と閉線分が共通点を持つか判定する。

## API別の時間計算量・空間計算量

- `intersect_circle_segment`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 接する場合と、端点で交わる場合も `true` を返す。
- 円の中心座標と半径は有限値でなければならない。非有限値または負の半径には `std::invalid_argument` を送出する。
- 中心から線分までの最小距離の二乗が半径の二乗以下で、かつ最大距離の二乗が半径の二乗以上のとき交差すると判定する。これらの比較は、各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で行う。中間値は有限な `long double` に収まる必要がある。
- `segment` の方向ベクトルの長さが `GEOMETRY_EPS` 以下なら、`segment.a` の 1 点として同じ規則で判定する。
