---
title: 2D Line Cross Point (二次元直線交点) [CGL_2_C]
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/line_line/cross_point.hpp
---

## API

- `Point cross_point(const Line& first, const Line& second)`: 2 本の無限直線の一意な交点を返す。

## API別の時間計算量・空間計算量

- `cross_point`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

方向ベクトルの長さが `GEOMETRY_EPS` 以下の退化直線には `std::invalid_argument` を送出する。平行または一致していて交点が一意に定まらない場合は `std::domain_error` を送出する。
方向ベクトルを単位ベクトルに正規化してから計算するので、退化判定に該当しない範囲では、各直線の方向ベクトルを正の定数倍しても交点は変わらない。
