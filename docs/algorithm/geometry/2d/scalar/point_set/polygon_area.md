---
title: 2D Polygon Area (二次元多角形の面積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set/polygon_area.hpp
---

多角形の面積を、頂点の向きによらない非負の値として求める。

## API

```cpp
long double polygon_area(const std::vector<Point>& polygon);
```

## 引数

周回順に並べた頂点列を渡す。

## 戻り値

頂点の向きによらない非負の面積（`area(polygon)` の絶対値）。

## 時間計算量

頂点数を $N$ として、時間計算量 $O(N)$、追加領域 $O(1)$。

## 注意点

自己交差する多角形では、符号付き面積の絶対値となる。
