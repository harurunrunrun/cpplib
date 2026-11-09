---
title: Convex Polyhedron Ray Interval (凸多面体と半直線の交差区間)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/convex_polyhedron_ray_interval.hpp
---

## API

- `convex_polyhedron_ray_interval(polyhedron, ray)`: 半直線 $o + t(p - o)$, $t \ge 0$ （$o, p$ は `ray.origin`, `ray.through`）が立体に含まれるパラメータ $t$ の閉区間を `std::optional<std::pair<long double, long double>>` で返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として、時間 $O(V + F)$、追加領域 $O(1)$。

## 注意点

- 交差しなければ `std::nullopt` を返す。
- 立体と半直線に対する前提・例外は直線版の `convex_polyhedron_line_interval` と同じである。
