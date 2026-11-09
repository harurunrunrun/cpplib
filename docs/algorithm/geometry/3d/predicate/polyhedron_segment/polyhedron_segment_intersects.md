---
title: General Polyhedron-Segment Intersection Predicate (一般多面体と線分の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_segment/polyhedron_segment_intersects.hpp
---

## API

- `polyhedron_segment_intersects(polyhedron, segment)`: 非凸でもよい多面体の閉領域と閉線分 `segment` が共有点を持つかを返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F)$、追加空間 $O(1)$。

## 注意点

- 端点のいずれかが多面体に含まれるか（`polyhedron_contains` と同じ判定）と、線分がいずれかの三角形面と交わるかを調べる。凸性は要求しない。
- 閉じた一貫した向きの非退化三角形メッシュと有限な線分を前提とする。非有限な頂点・線分や同じ頂点を重複して使う面には `std::invalid_argument`、範囲外の頂点インデックスには `std::out_of_range` を送出する。
- 面がなければ `false` を返す。
