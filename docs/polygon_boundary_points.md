---
title: Polygon Boundary Points
documentation_of: ../src/algorithm/geometry/polygon_boundary_points.hpp
---

与えられた点のうち、多角形の辺上にある点を列挙する。

# 関数

```cpp
enumerate_points_on_polygon_boundary(polygon, points, unique)
```

`polygon` は多角形の頂点列。`points` は調べる点の列。

返り値は `points` の index の列。多角形の辺を頂点列の順に見て、同じ辺上では辺の始点から近い順に並べる。

`unique = true` のとき、頂点上の点のように複数の辺に含まれる点は最初に現れた位置だけに出る。

## 時間計算量

- $O(NM \log M)$
