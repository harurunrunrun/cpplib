---
title: Line-General Polygon Intersection Test (直線と一般多角形の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/line_point_set/intersect_line_polygon.hpp
---

凸性を仮定しない多角形の閉領域と無限直線が交差するかを判定する。

## API

```cpp
bool intersect_line_polygon(
    const Line& line,
    const std::vector<Point>& polygon
);
```

全頂点の直線に対する向きを走査する。頂点が直線上にあるか、正負両側に頂点があれば真。

## 時間計算量

頂点数を $N$ とする。

- 時間計算量: $O(N)$
- 追加空間計算量: $O(1)$

## 注意点

空多角形には偽を返す。非有限座標、または2点が一致する退化直線には
`std::invalid_argument` を送出する。単純多角形だけでなく自己交差頂点列にも使える。
