---
title: Triangle Area from Two Sides and Angle (二辺と挟角から求める三角形面積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/scalar/triangle_area_from_two_sides_angle.hpp
---

2 辺の長さとその挟角から三角形の面積を求める。

## API

```cpp
long double triangle_area_from_two_sides_angle(
    long double first,
    long double second,
    long double angle
);
```

## 引数

有限かつ非負の 2 辺の長さ `first`, `second` と、ラジアン単位の挟角 `angle` を渡す。

## 戻り値

$\lvert \mathit{first} \cdot \mathit{second} \cdot \sin(\mathit{angle}) \rvert / 2$ を返す。

## 計算量

時間 $O(1)$、追加空間 $O(1)$。

## 注意点

引数に非有限値がある場合、または辺長が負の場合は `std::invalid_argument` を送出する。
