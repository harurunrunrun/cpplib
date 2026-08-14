---
title: Point Set Maximum Area Triangle (点集合内最大面積三角形)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/point_set_maximum_area_triangle.hpp
---

凸性や頂点順を仮定せず、入力点から選べる最大面積三角形を返す。最適解が凸包頂点で達成される性質を利用する。

## API

- `point_set_maximum_area_triangle(points)`: 選んだ3点と面積を返す。非退化な3点がなければ `valid == false`。

## API別の時間計算量・空間計算量

入力点数を $N$、凸包頂点数を $H$ とする。

- 凸包構築の時間計算量: $O(N\log N)$
- 最大三角形探索の時間計算量: $O(H^2)$
- 追加空間計算量: $O(N)$

## 注意点

入力座標は有限である必要がある。多角形境界だけでなく、任意順の点集合にも使用できる。
