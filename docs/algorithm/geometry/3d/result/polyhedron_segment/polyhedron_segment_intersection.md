---
title: General Polyhedron-Segment Intersection (一般多面体と線分の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron_segment/polyhedron_segment_intersection.hpp
---

凸性を仮定しない多面体の閉領域と線分の共通部分を、線分上の順に並んだ互いに素な線分列として返す。
非凸または非連結な立体では複数区間を返し、接点だけの共通部分は退化線分で表す。

## API

- `polyhedron_segment_intersection(polyhedron, segment)`: 共通部分を表す `std::vector<Segment3>` を返す。

## API別の時間計算量・空間計算量

面数を $F$、異なる境界交点parameter数を $K$ とする。

- 面交差収集: $O(F)$
- event整列: $O(K\log K)$
- 各区間の一般化立体角判定: $O(KF)$
- 追加空間計算量: $O(K)$

単純な横断では $K$ が小さいため線形に近く、最悪時間計算量は $O(F^2)$ となる。

## 注意点

閉じた一貫した向きの非退化三角形メッシュを前提とする。有限線分を要求し、境界を共通部分に含める。
