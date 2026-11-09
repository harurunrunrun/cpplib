---
title: 2D Ray-Point Distance (二次元半直線と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/ray_point/distance_ray_point.hpp
---

半直線と点の距離を求める。

## API

```cpp
long double distance(const Ray& ray, const Point& point);
```

## 戻り値

点から、半直線上の最近点までの距離。点の射影が始点より後ろ（または始点と同じ位置）にある場合は始点までの距離になる。

## 計算量

時間 $O(1)$、追加空間 $O(1)$。

## 注意点

- 退化した半直線（`origin` と `through` の距離が許容誤差以下）には `std::invalid_argument` を送出する。
- 退化と判定されない範囲では、`through - origin` を正の定数倍しても距離は変わらない。
