---
title: 2D Circumcircle (二次元外接円) [CGL_7_C]
documentation_of: ../../../../../../src/algorithm/geometry/2d/circle/point_point_point/circumcircle.hpp
---

## API

- `Circle circumcircle(first, second, third)`: 3 点を通る外接円を返す。

## API別の時間計算量・空間計算量

- `circumcircle`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

中心は `circumcenter(first, second, third)` で求め、半径は外心から `first` への差分の長さを `hypot` で求める。
3 点が共線の場合は `circumcenter` が `std::invalid_argument` を送出する。
