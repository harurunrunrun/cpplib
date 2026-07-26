---
title: Convex Polyhedron Intersection Internal Detail (凸多面体共通部分内部実装)
documentation_of: ../../../../src/algorithm/geometry/3d/convex_polyhedron_intersection_detail.hpp
---

半空間を使う二つの公開leafで共有する内部変換を保持する。

## API

- `convex_polyhedron_intersection_detail::append_halfspaces(polyhedron, halfspaces)`: 多面体の外向き三角形面を半空間列へ追加する内部関数。
- `convex_polyhedron_intersection_detail::combined_halfspaces(first, second)`: 両多面体の半空間列を返す内部関数。

## API別の時間計算量・空間計算量

$H=F_1+F_2$ として、`combined_halfspaces` は時間・返り値領域ともに $O(H)$。`append_halfspaces` は追加する面数を $F$ として時間 $O(F)$、追加領域 $O(F)$。

## 注意点

内部実装用headerであり、名前・引数・返り値の互換性は公開APIではない。入力は完全三次元で非空の頂点・面を持つ必要があり、違反時は `std::invalid_argument` を送出する。
