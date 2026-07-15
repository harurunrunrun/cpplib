---
title: Convex Polyhedron Diameter (凸多面体の直径)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_diameter.hpp
---

## API

- `convex_polyhedron_diameter(polyhedron)`: 最遠頂点対の添字と距離を返す。頂点が2個未満なら `std::nullopt`。

## API別の時間計算量・空間計算量

- 頂点数を $V$ として時間 $O(V^2)$、追加領域 $O(1)$。

## 注意点

凸多面体の直径は頂点対で達成される。非有限頂点を拒否する。
