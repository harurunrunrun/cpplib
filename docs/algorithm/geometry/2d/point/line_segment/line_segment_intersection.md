---
title: Line-Segment Intersection Point (直線と線分の交点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/line_segment/line_segment_intersection.hpp
---

## API

`Point line_segment_intersection(const Line& line, const Segment& segment)`

## 引数

直線と線分を渡す。

## 戻り値

直線と線分の一意な交点を返す。線分が 1 点に退化している場合は、その点が直線上にあればその点を返す。

## API別の時間計算量・空間計算量

- `line_segment_intersection`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化直線では `std::invalid_argument` を、交点がない場合や交点が無数にある（線分が直線上にある）場合は `std::domain_error` を送出する。
退化判定に該当しない範囲では、直線の方向ベクトルを正の定数倍しても交点は変わらない。
