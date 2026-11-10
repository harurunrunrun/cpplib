---
title: 2D Triangle Area (二次元三角形の面積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_point_point/triangle_area.hpp
---

3 点を頂点とする三角形の面積を求める。

## API

```cpp
long double triangle_area(
    const Point& first,
    const Point& second,
    const Point& third
);
```

## 引数

三角形の 3 頂点を渡す。

## 戻り値

頂点の向きによらない非負の面積（`triangle_signed_area` の絶対値）。

## 時間計算量

時間計算量 $O(1)$、追加領域 $O(1)$。

## 注意点

共線な 3 点に対しては `0` を返す。
