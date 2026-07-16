---
title: 3D Triangle BVH (三次元三角形境界体積階層)
documentation_of: ../src/algorithm/geometry/3d/triangle_bvh_3d.hpp
---

`TriangleBVH3D` は三角形のAABBを重心の最大spread軸で中央値分割する静的BVHである。
AABB broad-phase検索と、三角形との最近交点を返すray castingを提供する。

## API

- `TriangleBVH3D()`: 空のBVHを構築する。
- `TriangleBVH3D(triangles, leaf_capacity = 4)`: 三角形列をcopyして構築する。
- `build(triangles)`: 内容を破棄して再構築する。
- `size()`, `empty()`: 三角形数と空判定を返す。
- `triangle(index)`: 元のindexの三角形を返す。範囲外なら `std::out_of_range`。
- `aabb_query(range)`: 自身のAABBが `range` と接触または交差する三角形indexを
  昇順で返す。これはbroad-phaseであり、三角形そのものと箱の厳密な交差判定ではない。
- `ray_cast(ray, minimum_parameter = 0, maximum_parameter = infinity)`:
  `ray.origin + t * (ray.through - ray.origin)` と三角形の交点のうち最小の `t` を、
  `{triangle_index, t, point}` で返す。なければ `nullopt`。表裏両面を判定する。

## API別の時間計算量・空間計算量

- `build` と構築子: $O(N\log N)$ 時間、$O(N)$ 領域。
- `aabb_query`: 木の探索は平均 $O(\log N+K)$、最悪 $O(N)$。返却indexを
  昇順へsortするため、全体は平均 $O(\log N+K\log(K+1))$、最悪
  $O(N+K\log(K+1))$ 時間、返却値 $O(K)$。
- `ray_cast`: 平均 $O(\log N+H)$、最悪 $O(N)$ 時間、再帰stack $O(\log N)$。
- `size`, `empty`, `triangle`: $O(1)$ 時間・追加領域。

## 注意点

- 三角形、ray、AABBは有限でなければならない。`leaf_capacity` は正。
- `minimum_parameter` は有限かつ非負、`maximum_parameter` はNaNでなく
  `minimum_parameter` 以上とする。違反時は `std::invalid_argument`。
- 退化三角形とrayが三角形面に含まれる非一意な共通部分はhitとして返さない。
- 境界辺・頂点のhitを含み、同じparameterなら元index最小を返す。
