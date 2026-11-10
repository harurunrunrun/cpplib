---
title: General Polyhedra Intersection Predicate (一般多面体同士の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_polyhedron/polyhedra_intersect.hpp
---

凸性を仮定しない 2 つの閉三角形メッシュについて、それぞれが囲む閉領域が共有点を持つかを判定する。
面同士の交差候補は AABB 階層で絞り込み、表面同士が交差しない包含関係も表面の連結成分ごとに判定する。

## API

- `polyhedra_intersect(first, second)`: 接触・包含を含め、2 つの立体が交差するなら `true` を返す。

## API別の時間計算量・空間計算量

面数を $F, G$、頂点数を $V_1, V_2$、表面の連結成分数を $C_1, C_2$ とする。

- AABB 階層の構築: 時間 $O(F \log F + G \log G)$
- 面の組の候補探索: 通常は AABB により枝刈りされるが、最悪時間 $O(FG)$
- 表面が交差しない場合の包含判定: 時間 $O(V_1+V_2+C_1 G+C_2 F)$
- 追加空間: $O(F+G+V_1+V_2)$

## 注意点

- 各入力は有限な頂点からなる、閉じた一貫した向きの非退化三角形メッシュとする。凸性と表面の連結性は要求しない。
- 非有限な頂点や同じ頂点を重複して使う面には `std::invalid_argument`、範囲外の頂点インデックスには `std::out_of_range` を送出する。
- どちらかに面がなければ `false` を返す。
