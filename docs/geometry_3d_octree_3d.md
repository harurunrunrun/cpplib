---
title: 3D Octree (三次元八分木)
documentation_of: ../src/algorithm/geometry/3d/octree_3d.hpp
---

`Octree3D` は指定した有限な `AABB3` を各軸の中点で8分割する動的点集合である。
leafの点数が閾値を超えたときだけ子nodeを作る。

## API

- `Octree3D(bounds, leaf_capacity = 8, max_depth = 32)`: 空の木を構築する。
- `insert(point)`: 点を挿入し、0始まりの不変indexを返す。
- `size()`, `empty()`: 点数と空判定を返す。
- `bounds()`: root境界を返す。
- `point(index)`: 挿入indexの点を返す。範囲外なら `std::out_of_range`。
- `range_query(range)`: 閉じたAABB内の点indexを昇順で返す。
- `nearest(query)`: 最近点の `{index, distance}` を返す。空なら `nullopt`、
  同距離ならindex最小を返す。queryはroot外でもよい。

## API別の時間計算量・空間計算量

- 構築子、`size`、`empty`、`bounds`、`point`: $O(1)$ 時間。
- `insert`: 均等分布では平均 $O(D)$、最悪 $O(D+C)$ 時間。`D` は設定した
  最大深さ、`C` は分割不能leafの点数。
- `range_query`: 平均 $O(D+K)$、最悪 $O(N)$ 時間、返却値 $O(K)$。
- `nearest`: 平均 $O(D)$、最悪 $O(N)$ 時間。探索heapは最悪 $O(N)$ 領域。
- 木全体は $O(ND)$ nodeになり得るが、通常は $O(N)$ 領域。

## 注意点

- `bounds` と挿入点は有限で、挿入点は閉じたroot境界内でなければならない。
- `leaf_capacity` は正、`max_depth` は64以下とする。違反時は
  `std::invalid_argument`。
- 同一点を複数回挿入できる。最大深さで分割を止めるため無限再帰しない。
- 境界面上の点は一意な子へ保存するが、rangeは閉集合として判定する。
