---
title: Convex Hull Area (凸包面積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set/convex_hull_area.hpp
---

点集合の凸包の面積を求める。

## API

```cpp
long double convex_hull_area(std::vector<Point> points);
```

## 引数

`points` は重複を含んでもよい点集合である。

## 戻り値

非負の面積を `long double` で返す。

## 時間計算量

点数を $N$ として、時間計算量 $O(N \log N)$、空間計算量 $O(N)$。

## 注意点

点が 2 個以下の場合や全点が共線の場合は 0 を返す。
