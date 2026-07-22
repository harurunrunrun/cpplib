---
title: Maximum Noncrossing Bipartite Segments (二直線間線分の最大非交差部分集合) [BRIDGE]
documentation_of: ../src/algorithm/other/dynamic_programming/maximum_noncrossing_bipartite_segments.hpp
---

平行な2直線上の点を結ぶ線分から、内部で交差しない最大個数を求める。

## `maximum_noncrossing_bipartite_segments`

```cpp
template<class Coordinate>
size_t maximum_noncrossing_bipartite_segments(
    vector<pair<Coordinate, Coordinate>> segments
);
```

## 引数

- `segments[i].first`: 第1直線上の端点座標。
- `segments[i].second`: 第2直線上の端点座標。

## 戻り値

内部で交差しないように選べる線分の最大本数。同じ端点で接する線分は
交差とみなさない。

## API別の時間計算量・空間計算量

線分の本数を $N$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_noncrossing_bipartite_segments` | $O(N\log(N+1))$ | $O(N)$ |

## 注意点

- `Coordinate` は全順序で比較できなければならない。
