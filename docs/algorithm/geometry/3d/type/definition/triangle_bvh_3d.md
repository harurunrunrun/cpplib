---
title: 3D Triangle BVH (三次元三角形境界体積階層)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/triangle_bvh_3d.hpp
---

`TriangleBVH3D` は、三角形の AABB を、重心の広がりが最大の軸に沿って中央値で分割する静的な BVH である。
AABB による粗い候補検索（ブロードフェーズ）と、三角形との最も近い交点を返すレイキャストを提供する。

## API

- `TriangleBVH3D()`: 空の BVH を構築する。
- `TriangleBVH3D(triangles, leaf_capacity = 4)`: 三角形列をコピーして構築する。`leaf_capacity` は葉に入れる三角形数の上限。
- `build(triangles)`: 現在の内容を破棄し、`triangles` で再構築する（`leaf_capacity` は維持する）。
- `size()`, `empty()`: 三角形の個数と、空かどうかを返す。
- `triangle(index)`: 元のインデックス `index` の三角形を返す。範囲外なら `std::out_of_range` を送出する。
- `aabb_query(range)`: 三角形自身の AABB が `range` と接するか交わる三角形のインデックスを、昇順で返す。これはブロードフェーズであり、三角形そのものと箱との厳密な交差判定ではない。
- `ray_cast(ray, minimum_parameter = 0, maximum_parameter = infinity)`: `ray.origin + t * (ray.through - ray.origin)` と三角形との交点のうち、$t$ が最小のものを `std::tuple{triangle_index, t, point}` で返す。交点がなければ `std::nullopt` を返す。三角形の表裏の両面を判定する。

## API別の時間計算量・空間計算量

三角形の個数を $N$、`aabb_query` が返す個数を $K$、`ray_cast` で交差判定を行う三角形の個数を $H$ とする。

- `build` とコンストラクタ: 時間 $O(N \log N)$、領域 $O(N)$。
- `aabb_query`: 木の探索は平均 $O(\log N + K)$、最悪 $O(N)$。返すインデックスを昇順にソートするため、全体では平均 $O(\log N + K \log (K + 1))$、最悪 $O(N + K \log (K + 1))$ 時間、戻り値の領域 $O(K)$。
- `ray_cast`: 平均 $O(\log N + H)$、最悪 $O(N)$ 時間、再帰スタック $O(\log N)$。
- `size`, `empty`, `triangle`: 時間・追加領域ともに $O(1)$。

## 注意点

- 三角形・半直線・AABB は有限でなければならない。有限でない三角形を渡すと `build` とコンストラクタは `std::invalid_argument` を送出する。`aabb_query` に不正な `range` を渡した場合は `AABB3` の検査に従って例外を送出する。
- `leaf_capacity` は正でなければならず、0 なら `std::invalid_argument` を送出する。
- `ray_cast` では、`ray.origin == ray.through` の退化した半直線、`minimum_parameter` が有限かつ非負でない場合、`maximum_parameter` が NaN または `minimum_parameter` 未満の場合に `std::invalid_argument` を送出する。
- 退化した三角形との交差や、半直線が三角形の面に含まれて共通部分が一意に定まらない場合は、ヒットとして返さない。
- 三角形の境界の辺・頂点でのヒットも含む。同じパラメータで複数ヒットした場合は、元のインデックスが最小のものを返す。
