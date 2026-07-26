---
title: Segment-Segment Distance (線分同士の距離)
documentation_of: ../../../../src/algorithm/geometry/2d/segment_segment_distance.hpp
---

2本の閉線分の最短距離を求める。

## API

- `distance(first, second)`: 閉線分 `first` と `second` の距離を返す。

## API別の時間計算量・空間計算量

- `distance`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

交差する場合は0を返す。長さ0の退化線分を許す。戻り値の型は `long double` である。
