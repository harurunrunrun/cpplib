---
title: Convex Polyhedron Counts Result (凸多面体の要素数結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/convex_polyhedron_counts3.hpp
---

凸多面体の頂点・辺・面の個数を保持する結果型。

## API

- `vertices`: 頂点数。
- `edges`: 幾何学的な辺の数。
- `triangular_faces`: 後方互換用の格納三角形数。`triangulated_face_count` と同じ値を保持する。
- `facet_count`: 共面な隣接三角形をまとめた幾何面数。
- `triangulated_face_count`: 多面体に格納された三角形数。

## API別の時間計算量・空間計算量

- 各フィールドの読み書き: 時間・追加領域ともに $O(1)$。
- 結果型が保持する領域: $O(1)$。

## 注意点

この型自身は各個数の整合性を検査しない。`convex_polyhedron_counts` が返す値では `triangular_faces == triangulated_face_count` が成り立つ。
