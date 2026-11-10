---
title: Half-Plane Intersection Area (半平面共通部分の面積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/half_plane/half_plane_intersection_area.hpp
---

半平面の共通部分の面積を求める。

## API

```cpp
long double half_plane_intersection_area(const std::vector<Line>& half_planes);
```

## 引数

各 `Line{a, b}` は、`a` から `b` への向きに対して左側の閉半平面を表す。その境界直線の列を渡す。

## 戻り値

共通部分が空なら `0`、有界なら非負の面積、空でなく非有界なら正の無限大を返す。

## 時間計算量

半平面の数を $N$ として、時間計算量 $O(N \log N)$、空間計算量 $O(N)$。

## 注意点

- 境界の 2 点が一致する（退化した）場合や座標が有限でない場合は `std::invalid_argument` を、内部で用いる包含領域の計算が表現範囲を超える場合は `std::overflow_error` を送出する。
- 退化判定に該当しない範囲では、各境界の方向ベクトルを正の定数倍しても共通部分と面積は変わらない。
