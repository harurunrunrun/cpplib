---
title: Convex Polyhedron Query Hierarchy 3D (三次元凸多面体クエリ階層)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/convex_polyhedron_query_hierarchy_3d.hpp
---

同じ閉凸多面体に対する支持点・包含・最近点・線分との共通部分のクエリを多数処理する。三次元の支持点と包含には、球面三角形分割を低次数の独立集合で縮約する Dobkin--Kirkpatrick 型の階層を用いる。最近点には、厳密な距離の下界を持つ AABB 階層を用いる。

このヘッダは 5 つのメンバ関数実装の個別ヘッダをまとめて include する、後方互換のための集約ヘッダであり、利用側には従来どおり `ConvexPolyhedronQueryHierarchy3D` の完全な定義を提供する。

包含用の階層では、全頂点の平均を分母なしの厳密な 2 進有理数の式で表し、そこから境界メッシュを放射状に投影した三角形分割を縮約する。支持点用の階層では、共面な三角形を facet に統合し、facet の法線を頂点とする極的な normal fan を縮約する。各レベルでは次数 11 以下の頂点からなる極大独立集合を削除し、その link を厳密な行列式で再三角形分割する。粗い側の三角形が持つ細かい側の子の数は最大 11 で、最上位のレベルは頂点数 24 以下に固定され、これらは `leaf_capacity` に依存しない。

## API

- `ConvexPolyhedronQueryHierarchy3D(polyhedron, leaf_capacity = 8)`: `polyhedron` を所有するクエリオブジェクトを構築する（`explicit`）。三次元のメッシュで面の向きが内向きに統一されている場合は、全面を反転して正規化する。`leaf_capacity` は正でなければならず、最近点用の AABB と低次元での支持点探索にだけ使われる。
- `polyhedron()`: 検査・向きの正規化を行った後の `ConvexPolyhedron3` を参照する。
- `vertex_count()`: 頂点数を返す。
- `face_count()`: 三角形面の数を返す。
- `support_hierarchy_depth()`: 三次元の支持点用の極階層のレベル数を返す。低次元では 0 を返す。
- `contains_hierarchy_depth()`: 三次元の包含用の階層のレベル数を返す。低次元では 0 を返す。
- `hierarchy_maximum_branching()`: 両階層の粗い三角形が持つ子の数の最大値を返す。レベルが 1 つだけなら 0 を返す。
- `support_point(direction)`: `direction` との内積を最大にする頂点を返す。そのような頂点が複数ある場合は、`polyhedron().vertices` 内の添字が最小のものを返す。normal fan の facet 頂点、元の多面体の辺、セル内の人工的な対角線を区別して同点を処理する。
- `contains(point)`: 閉凸多面体に `point` が含まれるかを返す。境界上の点は含まれるとみなす。
- `closest_point(point)`: `point` が内部ならその点自身を、そうでなければ閉凸多面体上の最近点を返す。最近点が複数ある場合にどれを選ぶかは規定しない。
- `segment_intersection(segment)`: 線分との共通部分を返す。1 点で接する場合は退化した線分、空集合は `std::nullopt` で表す。面の AABB 木での slab 判定には、厳密な 2 進有理数の比較を使う。

## API 別の時間計算量・空間計算量

頂点数を $V$、三角形面の数を $F$、$N = V + F$ とする。多倍長の 2 進有理数の演算を 1 回と数えた算術演算量であり、ビット計算量は座標のビット長に依存する。

- コンストラクタ: 時間 $O(N \log N)$、空間 $O(N)$。放射状の各レベルで辺の接続関係に `std::map` を使うため、線形時間の前処理は主張しない。レベルの大きさは幾何級数的に減少する。
- `polyhedron`, `vertex_count`, `face_count`, `support_hierarchy_depth`, `contains_hierarchy_depth`, `hierarchy_maximum_branching`: 時間・追加空間ともに $O(1)$。
- 三次元の `support_point`: 最悪時間 $O(\log F) = O(\log V)$、追加空間 $O(1)$。normal fan の境界上での同点の処理もこの計算量に含まれる。
- 0・1・2 次元の `support_point`: AABB の枝刈りで訪れるノード数を $K_V$ として時間 $O(K_V)$、最悪 $O(V)$。再帰のスタックは $O(\log V)$。
- 三次元の `contains`: 最悪時間 $O(\log V)$、追加空間 $O(1)$。最も細かいレベルの面を特定した後、その面について厳密な半空間判定を 1 回行う。
- 0・1・2 次元の `contains`: 既存の低次元 API にフォールバックし、最悪時間 $O(F)$。
- 三次元の `closest_point`: 面の AABB 階層で訪れるノード数を $K_F$ として時間 $O(\log V + K_F)$、最悪 $O(F)$。再帰のスタックは $O(\log F)$。最近点については、Dobkin--Kirkpatrick 型階層による最悪対数時間は主張しない。
- 二次元の `closest_point`: 包含判定のフォールバックを含めて最悪時間 $O(F)$。0・1 次元では時間 $O(1)$。
- 三次元の `segment_intersection`: 面の AABB 木で訪れるノード数を $K_S$ として時間 $O(\log V + K_S)$、最悪 $O(F)$。探索スタックは $O(\log F)$。二次元では $O(K_S)$、0・1 次元では $O(1)$ 時間。

球面三角形分割の平均次数から、次数 12 以上の頂点は全体の半分未満である。次数 11 以下の候補に対する貪欲な極大独立集合は候補数の $1/12$ 以上なので、頂点数が 24 を超える各レベルでは全頂点の $1/24$ より多くが削除される。したがって、レベル数は最悪 $O(\log N)$、全レベルの三角形数と子の数の総和は $O(N)$ である。

## 注意点

- すべての座標とクエリは有限値でなければならない。支持方向は零ベクトルであってはならない。違反時は `std::invalid_argument` を送出する。
- `affine_dimension == 3` では、各面が非退化な三角形で、各辺をちょうど 2 つの面が逆向きに共有する、閉じていて向きが一貫したメッシュを要求する。添字、非有限値、面の退化、閉メッシュ条件、使われない頂点、面隣接グラフの非連結性を構築時に検査し、違反時は `std::invalid_argument`、`std::out_of_range` または `std::domain_error` を送出する。
- 三次元メッシュが幾何的に凸であり、各面が境界を正しく三角形分割していることは前提とする。共面な facet の内部や辺上に追加のメッシュ頂点があってもよい。
- 放射状の各レベルでは、すべての辺が 2 面に共有されること、オイラーの公式、正の向きの錐、独立集合による削減率、子の添字を検査する。不変条件を満たさない入力を、黙って線形時間のクエリにフォールバックさせることはしない。
- `affine_dimension` は実際のアフィン次元と一致しなければならない。0・1 次元では面を持たず、二次元では非退化な三角形面を持つことを要求する。
- 一次元では `vertices.front()` と `vertices.back()` を線分の両端とする。
- 支持点と包含の符号判定では、入力の `long double` を正確な 2 進有理数として扱う。最近点の座標の計算精度は、既存の `closest_point(Triangle3, Point3)` と同じである。
