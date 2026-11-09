---
title: Convex Hull Perimeter (凸包周長)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set/convex_hull_perimeter.hpp
---

点集合の凸包の周長を求める。

## API

```cpp
long double convex_hull_perimeter(std::vector<Point> points);
```

## 引数

`points` は重複を含んでもよい点集合である。

## 戻り値

凸包の周長を `long double` で返す。

## 時間計算量

点数を $N$ として、時間計算量 $O(N \log N)$、空間計算量 $O(N)$。

## 注意点

空集合と 1 点（重複のみの場合を含む）では 0 を、凸包が線分に退化する場合はその線分の長さの 2 倍を返す。
