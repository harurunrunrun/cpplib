---
title: Triangle Area from Two Sides and Angle (二辺と挟角から求める三角形面積)
documentation_of: ../src/algorithm/geometry/2d/triangle_area_from_two_sides_angle.hpp
---

## API

`long double triangle_area_from_two_sides_angle(first, second, angle)`

## 引数

有限な非負の2辺長とrad単位の挟角を渡す。

## 戻り値

`|first * second * sin(angle)| / 2`。

## API別の時間計算量・空間計算量

- `triangle_area_from_two_sides_angle`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

非有限値または負辺長では `invalid_argument`。
