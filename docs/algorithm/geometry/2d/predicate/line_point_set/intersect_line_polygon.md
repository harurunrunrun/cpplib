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

全頂点について直線に対する向きを走査する。直線上にある頂点が存在するか、直線の両側に頂点があれば `true` を返す。

## 時間計算量

頂点数を $N$ とする。

- 時間計算量: $O(N)$
- 追加空間計算量: $O(1)$

## 注意点

- 空の多角形には `false` を返す。
- 非有限な座標、または方向ベクトルの $x, y$ 成分がともに `GEOMETRY_EPS` 以下の退化直線には `std::invalid_argument` を送出する。
- 直線の左右判定は、方向ベクトルを正規化しない外積に対する絶対許容誤差 `GEOMETRY_EPS` で行う。
- 単純多角形だけでなく、自己交差する頂点列にも使える。
