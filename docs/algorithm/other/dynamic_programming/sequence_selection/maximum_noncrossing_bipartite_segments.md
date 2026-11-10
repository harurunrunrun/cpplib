---
title: Maximum Noncrossing Bipartite Segments (二直線間線分の最大非交差部分集合) [BRIDGE]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/sequence_selection/maximum_noncrossing_bipartite_segments.hpp
---

平行な 2 直線上の点を結ぶ線分の集合から、互いに内部で交差しないように選べる線分の最大本数を求める。第 1 直線上の座標でソートし、第 2 直線上の座標の最長非減少部分列として計算する。

## `maximum_noncrossing_bipartite_segments`

```cpp
template<class Coordinate>
std::size_t maximum_noncrossing_bipartite_segments(
    std::vector<std::pair<Coordinate, Coordinate>> segments
);
```

## 引数

- `segments[i].first`: 第 1 直線上の端点の座標。
- `segments[i].second`: 第 2 直線上の端点の座標。

## 戻り値

内部で交差しないように選べる線分の最大本数。端点を共有するだけの線分同士は交差とみなさない。

## API別の時間計算量・空間計算量

線分の本数を $N$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_noncrossing_bipartite_segments` | $O(N\log(N+1))$ | $O(N)$ |

## 注意点

- `Coordinate` は全順序で比較できなければならない。
