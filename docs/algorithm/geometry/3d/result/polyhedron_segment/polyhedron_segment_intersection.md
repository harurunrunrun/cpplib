---
title: General Polyhedron-Segment Intersection (一般多面体と線分の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron_segment/polyhedron_segment_intersection.hpp
---

凸性を仮定しない多面体の閉領域と線分の共通部分を、線分上の順に並んだ互いに素な線分の列として返す。非凸または非連結な立体では複数の区間を返し、接するだけの共通部分は退化線分で表す。

## API

- `polyhedron_segment_intersection(polyhedron, segment)`: 共通部分を表す `std::vector<Segment3>` を返す。両端点が完全に一致する退化線分は、点の包含判定として扱う。

## API別の時間計算量・空間計算量

面数を $F$、相異なる境界交点のパラメータの数を $K$ とする。

- 面との交差の収集: $O(F)$
- イベントのソート: $O(K \log K)$
- 各区間に対する一般化立体角による包含判定: $O(KF)$
- 追加空間計算量: $O(K)$

単純な横断では $K$ が小さいため線形に近い。最悪時間計算量は $O(F^2)$ となる。

## 注意点

- 閉じていて向きが一貫した、非退化な三角形メッシュを前提とする。多面体の検証に失敗した場合や、線分の端点が非有限の場合は `std::invalid_argument` を送出する。
- 境界も共通部分に含める。
