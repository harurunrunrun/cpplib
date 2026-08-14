---
title: General Polyhedra Intersection Predicate (一般多面体同士の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_polyhedron/polyhedra_intersect.hpp
---

凸性を仮定しない2つの閉三角形メッシュの閉領域が共有点を持つかを判定する。
面同士はAABB階層で候補を絞り、表面が交差しない包含ケースも連結成分ごとに判定する。

## API

- `polyhedra_intersect(first, second)`: 接触・包含を含め、2立体が交差するなら `true`。

## API別の時間計算量・空間計算量

面数を $F,G$、表面連結成分数を $C_1,C_2$ とする。

- AABB階層構築の時間計算量: $O(F\log F+G\log G)$
- 面候補探索: 通常は空間分割で枝刈りされ、最悪時間計算量 $O(FG)$
- 非交差表面の包含判定: $O(C_1G+C_2F)$
- 追加空間計算量: $O(F+G+V_1+V_2)$

## 注意点

各入力は有限頂点からなる、閉じた一貫した向きの非退化三角形メッシュとする。凸性と単一連結成分は要求しない。
