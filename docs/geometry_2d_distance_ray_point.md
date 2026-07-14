---
title: 2D Ray-Point Distance (二次元半直線と点の距離)
documentation_of: ../src/algorithm/geometry/2d/distance_ray_point.hpp
---

## API

`long double distance(const Ray& ray, const Point& point)`

## 引数

半直線と点を渡す。

## 戻り値

点から半直線上の最近点までの距離。

## API別の時間計算量・空間計算量

- `distance(Ray,Point)`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化半直線では `std::invalid_argument` を送出する。
