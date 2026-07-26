---
title: Convex Polyhedron Facet Result (凸多面体の幾何面結果)
documentation_of: ../../../../src/algorithm/geometry/3d/convex_polyhedron_facet3.hpp
---

三角形分割された凸多面体について、一つの幾何面を表す結果型。

## API

- `boundary`: 面の境界を巡る頂点添字列。
- `triangles`: この幾何面を構成する元の三角形添字列。

## API別の時間計算量・空間計算量

- 各フィールドへの参照: 時間・追加領域ともに $O(1)$。
- 境界長を $B$、三角形数を $T$ とした保持領域: $O(B+T)$。

## 注意点

この型自身は添字範囲や境界の向きを検査しない。`convex_polyhedron_facets` が返す `boundary` は外向きの境界ループで、`triangles` は入力多面体の面添字を指す。
