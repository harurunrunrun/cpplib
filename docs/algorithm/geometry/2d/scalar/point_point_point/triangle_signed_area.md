---
title: 2D Triangle Signed Area (二次元三角形の符号付き面積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_point_point/triangle_signed_area.hpp
---

3 点を頂点とする三角形の符号付き面積を求める。

## API

```cpp
long double triangle_signed_area(
    const Point& first,
    const Point& second,
    const Point& third
);
```

## 引数

三角形の 3 頂点を周回順に渡す。

## 戻り値

`cross(second - first, third - first) / 2`。頂点が反時計回りなら正、時計回りなら負の面積となる。

## 時間計算量

時間計算量 $O(1)$、追加領域 $O(1)$。

## 注意点

共線な 3 点に対しては `0` を返す。
