---
title: Point-Set Minimum Width (一般点集合の最小幅)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set/point_set_minimum_width.hpp
---

凸性を仮定しない点集合、または非凸多角形の全頂点を挟む平行2直線間距離の最小値を求める。

## API

```cpp
long double point_set_minimum_width(std::vector<Point> points);
```

凸包を構築し、rotating calipersで最小幅を求める。点または線分へ退化する場合は0。

## 時間計算量

入力点数を $N$、凸包頂点数を $H$ とする。

- 時間計算量: $O(N\log(N+1)+H)$
- 追加空間計算量: $O(N)$

## 注意点

入力順と凸性を問わない。自己交差する多角形の頂点列にも、点集合として定義した幅を返す。
