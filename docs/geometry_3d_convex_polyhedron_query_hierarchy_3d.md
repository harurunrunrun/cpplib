---
title: Convex Polyhedron Query Hierarchy 3D (三次元凸多面体クエリ階層)
documentation_of: ../src/algorithm/geometry/3d/query/convex_polyhedron_query_hierarchy_3d.hpp
---

同じ凸多面体に対する支持点、包含、最近点の多数queryを処理する。
頂点、面の半空間係数、面のAABBにそれぞれ二分階層を構築し、厳密な上界・下界で
候補を枝刈りする。単発query用APIは変更しない。

## API

- `ConvexPolyhedronQueryHierarchy3D(polyhedron, leaf_capacity = 8)`:
  `polyhedron` を所有するquery objectを構築する。内向きに統一された3次元meshは
  全面を反転して正規化する。`leaf_capacity` は正でなければならない。
- `polyhedron()`: 検査・向き正規化後の `ConvexPolyhedron3` を参照する。
- `vertex_count()`, `face_count()`: 頂点数、三角形面数を返す。
- `support_point(direction)`: `direction` との内積を最大化する頂点を返す。
  同値な頂点が複数ある場合は `polyhedron().vertices` 内のindexが最小のものを返す。
- `contains(point)`: 閉凸多面体に `point` が含まれるかを返す。境界は内部とする。
- `closest_point(point)`: `point` が内部ならその点自身、それ以外なら閉凸多面体上の
  最近点を返す。最近点が複数ある場合の選択は規定しない。

## API別の時間計算量・空間計算量

頂点数を $V$、三角形面数を $F$ とする。

- 構築: median分割と閉mesh検査を含め $O(V\log V+F\log F)$ 時間、
  $O(V+F)$ 領域。多倍長dyadic係数のbit計算量は入力値のbit長に依存する。
- `polyhedron`, `vertex_count`, `face_count`: $O(1)$ 時間・追加領域。
- `support_point`: 訪問した階層node数を $K_V$ として $O(K_V)$、
  最悪 $O(V)$ 時間。探索stackは $O(\log V)$。
- 3次元の `contains`: 訪問した階層node数を $K_H$ として $O(K_H)$、
  最悪 $O(F)$ 時間。0・1・2次元では既存APIへのfallbackにより最悪 $O(F)$。
- `closest_point`: 訪問した階層node数を $K_F$ として $O(K_F)$、
  最悪 $O(F)$ 時間。
  0・1次元では $O(1)$。内部点では `contains` の費用もかかる。
- 2次元では包含判定が低次元fallbackを使うため、`contains` と
  `closest_point` はともに最悪 $O(F)$ 時間。

この階層はDobkin--Kirkpatrick階層の $O(\log N)$ 最悪保証を主張しない。
一方、枝刈りの比較は丸めたAABB距離だけに依存せず、頂点座標を表す
`ExactDyadic` の内積・半空間上界・距離下界で確認する。このため、浮動小数点の
丸めによって真の候補を枝刈りすることはない。最近点の枝刈り上界には、丸め済みの
三角形最近点ではなく、必ずmesh上にある頂点までの厳密距離を使う。有限な
`long double` に対するこのAPI内のdyadic指数演算は `int` の範囲に収まり、仮数は
`ExactInteger` が保持する。

## 注意点

- 全座標とqueryは有限値でなければならない。支持方向は零vectorであってはならない。
- `affine_dimension == 3` では、各面が非退化三角形で、各辺をちょうど2面が逆向きに
  共有する閉じた一貫orientationのmeshを要求する。index、非有限値、面の退化、
  閉mesh条件、全頂点が少なくとも一面に属すること、面隣接graphが連結であることは
  構築時に検査し、違反時は `std::invalid_argument`、`std::out_of_range`
  または `std::domain_error` を送出する。
- 3次元meshが幾何学的に凸であり、各faceが境界を正しく三角形分割していることは
  前提である。これを全頂点と全面について検査すると前処理が $O(VF)$ になるため、
  構築子では再検査しない。
- `affine_dimension` は実際のaffine次元と一致しなければならない。0・1次元ではfaceを
  持たず、2次元では非退化三角形面を持つことを要求する。
- 1次元では `vertices.front()` と `vertices.back()` を線分の両端とする。
  これは `convex_hull_3d` が返す低次元表現と同じ契約である。
- 支持点と包含の順序判定は入力long doubleを正確なdyadic数として扱う。
  最近点の座標計算精度は既存の `closest_point(Triangle3, Point3)` と同じである。
