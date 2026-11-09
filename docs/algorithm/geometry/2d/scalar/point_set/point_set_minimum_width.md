---
title: Point-Set Minimum Width (一般点集合の最小幅)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set/point_set_minimum_width.hpp
---

凸性を仮定しない点集合（非凸多角形の頂点列を含む）について、全点を挟む平行な 2 直線間の距離の最小値を求める。

## API

```cpp
long double point_set_minimum_width(std::vector<Point> points);
```

凸包を構築し、rotating calipers で最小幅を求める。点または線分に退化する場合は 0 を返す。

## 時間計算量

入力点数を $N$ とする。

- 時間計算量: $O(N \log N)$（凸包の構築が支配的で、その後の rotating calipers は凸包の頂点数に対して線形）
- 追加空間計算量: $O(N)$

## 注意点

入力の順序と凸性は問わない。自己交差する多角形の頂点列に対しても、点集合として定義した幅を返す。
