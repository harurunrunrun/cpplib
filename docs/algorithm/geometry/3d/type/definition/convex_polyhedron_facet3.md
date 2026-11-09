---
title: Convex Polyhedron Facet Result (凸多面体の幾何面結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/convex_polyhedron_facet3.hpp
---

三角形分割された凸多面体について、1 つの幾何学的な面を表す結果型。

## API

- `boundary`: 面の境界を一周する頂点インデックスの列（`std::vector<std::size_t>`）。
- `triangles`: この面を構成する元の三角形のインデックスの列（`std::vector<std::size_t>`）。

## API別の時間計算量・空間計算量

- 各メンバの参照: 時間・追加領域ともに $O(1)$。
- 境界の長さを $B$、三角形の数を $T$ とすると、保持する領域は $O(B + T)$。

## 注意点

- この型自身はインデックスの範囲や境界の向きを検査しない。`convex_polyhedron_facets` が返す値では、`boundary` は外向きの境界ループであり、`triangles` は入力多面体の面インデックスを指す。
