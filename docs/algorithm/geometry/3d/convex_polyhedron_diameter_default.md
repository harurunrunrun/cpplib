---
title: Convex Polyhedron Diameter by Pair Scan (全頂点対走査による凸多面体直径)
documentation_of: ../../../../src/algorithm/geometry/3d/convex_polyhedron_diameter_default.hpp
---

## API

- `convex_polyhedron_diameter(polyhedron)`: 全頂点対を調べ、Euclidean直径を達成する頂点添字対と距離を `PointSetDiameter3DResult` で返す。頂点が2個未満なら `std::nullopt`。

## API別の時間計算量・空間計算量

頂点数を $V$ として時間 $O(V^2)$、追加領域 $O(1)$。

## 注意点

直径は凸多面体の頂点対で達成される。同距離なら添字対が辞書順最小のものを返す。非有限頂点には `std::invalid_argument` を送出する。
