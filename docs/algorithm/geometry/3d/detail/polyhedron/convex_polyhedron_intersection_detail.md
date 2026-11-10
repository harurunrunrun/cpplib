---
title: Convex Polyhedron Intersection Internal Detail (凸多面体共通部分内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/polyhedron/convex_polyhedron_intersection_detail.hpp
---

半空間を使う 2 つの公開個別ヘッダが共有する内部変換である。

## API

- `convex_polyhedron_intersection_detail::append_halfspaces(polyhedron, halfspaces)`: 多面体の各三角形面を、面の 1 頂点を通り外向きの法線（正規化しない外積）を持つ半空間として `halfspaces` に追加する内部関数。
- `convex_polyhedron_intersection_detail::combined_halfspaces(first, second)`: 両多面体の半空間を連結した列を返す内部関数。

## API別の時間計算量・空間計算量

2 つの多面体の面数を $F_1, F_2$、$H=F_1+F_2$ とする。`combined_halfspaces` は時間・返り値の領域ともに $O(H)$。`append_halfspaces` は追加する面数を $F$ として時間・追加領域ともに $O(F)$。

## 注意点

内部実装用のヘッダであり、名前・引数・返り値の互換性は公開 API として保証しない。入力はアフィン次元が 3 で、頂点と面が空でない必要がある。違反時や面の頂点添字が範囲外の場合は `std::invalid_argument` を送出する。
