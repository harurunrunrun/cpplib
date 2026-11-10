---
title: Convex Polyhedron Contains (凸多面体の点包含)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_point/convex_polyhedron_contains.hpp
---

## API

- `convex_polyhedron_contains(polyhedron, point)`: 点 `point` が凸形状 `polyhedron` に境界を含めて含まれるかを返す。`affine_dimension` が 3 未満の退化した凸形状にも対応する。

## API別の時間計算量・空間計算量

- 面数を $F$ として時間 $O(F)$、追加領域 $O(1)$。

## 注意点

- `affine_dimension` に応じて次のように判定する。
  - 頂点が空: `false` を返す。
  - 0 以下（1 点）: 先頭頂点との距離が `GEOMETRY3D_EPS` 以下なら `true`。
  - 1（線分）: 先頭頂点と末尾頂点を結ぶ線分上にあれば `true`。
  - 2（平面図形）: いずれかの三角形面に含まれれば `true`。
  - 3: すべての面について適応的 `orient3d` が正でなければ `true`。面は外向きであることを前提とする。
- `point` が非有限なら `std::invalid_argument`、面の頂点インデックスが範囲外なら `std::out_of_range` を送出する。
