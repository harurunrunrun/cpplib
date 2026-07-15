---
title: 3D kd-tree (三次元kd木)
documentation_of: ../src/algorithm/geometry/3d/kd_tree_3d.hpp
---

`KdTree3D` は有限な三次元点を最大spread軸の中央値で再帰分割し、各部分木の
`AABB3` を使って正確な最近傍探索を行う静的kd-treeである。

## API

- `KdTree3D()`: 空の木を構築する。
- `KdTree3D(points)`: `points` をcopyして木を構築する。
- `build(points)`: 内容を破棄し、点集合から再構築する。
- `size()`, `empty()`: 格納点数と空判定を返す。
- `point(index)`: 構築時のindexに対応する点を返す。範囲外なら
  `std::out_of_range`。
- `nearest(query)`: `{index, distance}` を返す。空なら `nullopt`。同距離なら
  index最小を選ぶ。
- `k_nearest(query, count)`: 距離、indexの順に安定して並べた高々 `count` 個の
  `{index, distance}` を返す。
- `radius_query(query, radius)`: 閉球内の全点を距離、index順で返す。

## API別の時間計算量・空間計算量

- `build` と構築子: 平均・最悪 $O(N\log N)$ 時間、$O(N)$ 領域。
- `nearest`: 平均 $O(\log N)$、最悪 $O(N)$ 時間、再帰stack $O(\log N)$
  （退化時も中央値分割なので木の高さは $O(\log N)$）。
- `k_nearest`: 平均 $O(\log N + K\log K)$、最悪 $O(N\log K)$ 時間、
  返却値とheapに $O(K)$ 領域。
- `radius_query`: 平均 $O(\log N + K)$、最悪 $O(N)$ 時間、返却値 $O(K)$。
- `size`, `empty`, `point`: $O(1)$ 時間・追加領域。

## 注意点

- 入力点とqueryは有限でなければならず、違反時は `std::invalid_argument`。
- `radius` は有限かつ非負でなければならない。
- 動的更新は行わない。更新後は `build` で再構築する。
- 距離とAABB下界には `std::hypot` を使い、平方距離のoverflowを避ける。

