---
title: Convex Polyhedron Query Hierarchy 3D (三次元凸多面体クエリ階層)
documentation_of: ../src/algorithm/geometry/3d/query/convex_polyhedron_query_hierarchy_3d.hpp
---

同じ閉凸多面体に対する支持点、包含、最近点の多数queryを処理する。
3次元の支持点と包含には、球面三角形分割を低次数独立集合で縮約する
Dobkin--Kirkpatrick型階層を用いる。最近点には厳密な距離下界を持つAABB階層を用いる。

包含用階層は、全頂点の平均を分母なしの厳密dyadic式で表し、そこから境界meshを
放射投影した三角形分割を縮約する。支持点用階層はcoplanarな三角形をfacetへ統合し、
facet法線を頂点とするpolar normal fanを縮約する。各levelでは次数11以下の頂点の
極大独立集合を削除し、linkを厳密determinantで再三角形分割する。coarse三角形が持つ
fine側child数は最大11、最上位は頂点数24以下に固定され、`leaf_capacity` に依存しない。

## API

- `ConvexPolyhedronQueryHierarchy3D(polyhedron, leaf_capacity = 8)`:
  `polyhedron` を所有するquery objectを構築する。内向きに統一された3次元meshは
  全面を反転して正規化する。`leaf_capacity` は正でなければならず、最近点用AABB
  および低次元の支持点探索だけに使われる。
- `polyhedron()`: 検査・向き正規化後の `ConvexPolyhedron3` を参照する。
- `vertex_count()`: 頂点数を返す。
- `face_count()`: 三角形面数を返す。
- `support_hierarchy_depth()`: 3次元support用polar階層のlevel数を返す。
  低次元では0を返す。
- `contains_hierarchy_depth()`: 3次元包含用primal階層のlevel数を返す。
  低次元では0を返す。
- `hierarchy_maximum_branching()`: 両階層のcoarse三角形が持つchild数の最大値を返す。
  levelが1つだけなら0を返す。
- `support_point(direction)`: `direction` との内積を最大化する頂点を返す。
  同値な頂点が複数ある場合は `polyhedron().vertices` 内のindexが最小のものを返す。
  normal-fanのfacet頂点、元polyhedron edge、cell内の人工対角線を区別してtieを処理する。
- `contains(point)`: 閉凸多面体に `point` が含まれるかを返す。境界は内部とする。
- `closest_point(point)`: `point` が内部ならその点自身、それ以外なら閉凸多面体上の
  最近点を返す。最近点が複数ある場合の選択は規定しない。

## API別の時間計算量・空間計算量

頂点数を $V$、三角形面数を $F$、$N=V+F$ とする。
多倍長dyadic整数の1演算を1回と数えた算術演算量である。bit計算量は座標のbit長に依存する。

- 構築子: $O(N\log N)$ 時間、$O(N)$ 領域。各radial levelでedge incidenceに
  `std::map` を使うため線形前処理は主張しない。levelの大きさは幾何級数的に減少する。
- `polyhedron`, `vertex_count`, `face_count`,
  `support_hierarchy_depth`, `contains_hierarchy_depth`,
  `hierarchy_maximum_branching`: $O(1)$ 時間・追加領域。
- 3次元の `support_point`: 最悪 $O(\log F)=O(\log V)$ 時間、
  $O(1)$ 追加領域。normal-fan境界上のtieも同じ計算量に含む。
- 0・1・2次元の `support_point`: AABB枝刈りの訪問node数を $K_V$ として
  $O(K_V)$、最悪 $O(V)$ 時間。再帰stackは $O(\log V)$。
- 3次元の `contains`: 最悪 $O(\log V)$ 時間、$O(1)$ 追加領域。
  finest faceを定位した後は、そのfaceの厳密な半空間判定を1回行う。
- 0・1・2次元の `contains`: 既存の低次元APIへfallbackし、最悪 $O(F)$ 時間。
- 3次元の `closest_point`: 面AABB階層の訪問node数を $K_F$ として
  $O(\log V+K_F)$、最悪 $O(F)$ 時間。再帰stackは $O(\log F)$。
  最近点についてDobkin--Kirkpatrick型階層の最悪対数時間は主張しない。
- 2次元の `closest_point`: 包含fallbackを含め最悪 $O(F)$ 時間。
  0・1次元では $O(1)$ 時間。

次数12以上の頂点は球面三角形分割の平均次数から全体の半分未満である。
次数11以下の候補上のgreedy極大独立集合は候補数の少なくとも $1/12$ なので、
頂点数24を超える各levelで全頂点の $1/24$ より多くを削除する。
したがってlevel数は最悪 $O(\log N)$、全levelの三角形数とchild数の総和は $O(N)$ である。

## 注意点

- 全座標とqueryは有限値でなければならない。支持方向は零vectorであってはならない。
- `affine_dimension == 3` では、各面が非退化三角形で、各辺をちょうど2面が逆向きに
  共有する閉じた一貫orientationのmeshを要求する。index、非有限値、面の退化、
  閉mesh条件、未使用頂点、面隣接graphの非連結を構築時に検査し、違反時は
  `std::invalid_argument`、`std::out_of_range` または `std::domain_error` を送出する。
- 3次元meshが幾何学的に凸であり、各faceが境界を正しく三角形分割していることは
  前提である。coplanar facetの内部やedge上に追加のmesh頂点があってもよい。
- radial levelでは全edgeの2面共有、Euler式、正向きcone、独立集合の削減率、
  child indexを検査する。不変条件を満たさない入力を線形queryへ黙ってfallbackしない。
- `affine_dimension` は実際のaffine次元と一致しなければならない。0・1次元ではfaceを
  持たず、2次元では非退化三角形面を持つことを要求する。
- 1次元では `vertices.front()` と `vertices.back()` を線分の両端とする。
- 支持点と包含の符号判定は入力 `long double` を正確なdyadic数として扱う。
  最近点の座標計算精度は既存の `closest_point(Triangle3, Point3)` と同じである。
