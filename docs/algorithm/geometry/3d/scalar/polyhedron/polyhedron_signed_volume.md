---
title: General Polyhedron Signed Volume (一般多面体の符号付き体積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/polyhedron/polyhedron_signed_volume.hpp
---

凸性を仮定しない閉じた三角形メッシュの符号付き体積を、向き付き四面体の体積の和で計算する。

## API

- `polyhedron_signed_volume(polyhedron)`: 面が外向きなら正、内向きなら負の体積を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として、時間 $O(V + F)$、追加領域 $O(1)$。

## 注意点

- 面が閉じていて、向きが一貫していることを前提とする。
- 非有限な頂点や同じ頂点を重複して含む面には `std::invalid_argument` を、範囲外の頂点添字には `std::out_of_range` を送出する。
- 結果が `long double` で表現できない場合は `std::overflow_error` を送出する。
