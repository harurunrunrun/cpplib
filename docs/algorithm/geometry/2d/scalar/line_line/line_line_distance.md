---
title: Line-Line Distance (直線同士の距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/line_line/line_line_distance.hpp
---

2 本の直線（無限に延びる直線）の間の最短距離を求める。

## API

```cpp
long double distance_line_line(const Line& first, const Line& second);
```

`first` と `second` の距離を返す。

## 時間計算量

時間計算量 $O(1)$、追加領域 $O(1)$。

## 注意点

交差する直線や一致する直線に対しては 0 を返す。平行な場合は `second.a` から `first` までの距離を返す。退化した直線には `std::invalid_argument` を送出する。
