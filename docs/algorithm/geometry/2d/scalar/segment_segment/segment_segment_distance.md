---
title: Segment-Segment Distance (線分同士の距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/segment_segment/segment_segment_distance.hpp
---

2 本の閉線分の最短距離を求める。

## API

```cpp
long double distance(const Segment& first, const Segment& second);
```

閉線分 `first` と `second` の距離を返す。

## 計算量

時間 $O(1)$、追加空間 $O(1)$。

## 注意点

- 2 本が交差（接触を含む）する場合は $0$ を返す。そうでなければ、各端点と相手の線分との距離 4 通りの最小値を返す。
- 長さ $0$ の退化線分も受け付ける。
