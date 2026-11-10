---
title: 2D Closest Pair Distance (二次元最近点対距離) [CGL_5_A]
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set/closest_pair.hpp
---

点集合の最近点対の距離を、分割統治法で求める。

## API

```cpp
long double closest_pair(std::vector<Point> points);
```

相異なる 2 つの入力点（添字が異なる 2 点）のユークリッド距離の最小値を返す。

## 時間計算量

点数を $N$ として、時間計算量 $O(N \log N)$、追加領域 $O(N)$。

## 注意点

点が 2 個未満の入力では正の無限大を、重複点があれば 0 を返す。
