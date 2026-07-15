---
title: Convex Polyhedron Support Point (凸多面体の支持点)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_support_point.hpp
---

## API

- `convex_polyhedron_support_point(polyhedron, direction)`: `dot(point, direction)` を最大化する頂点を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$ として時間 $O(V)$、追加領域 $O(1)$。

## 注意点

空形状、ゼロ方向、非有限値を拒否する。目的値は先頭頂点からの差に対するexact dyadic
内積で比較するため、巨大な共通平行移動量より局所差が非常に小さい場合にも支持点を
区別できる。最大点が複数なら頂点列で最初のものを返す。
