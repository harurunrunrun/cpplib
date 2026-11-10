---
title: General Polyhedron Volume (一般多面体の体積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/polyhedron/polyhedron_volume.hpp
---

## API

- `polyhedron_volume(polyhedron)`: 一般多面体の符号付き体積 `polyhedron_signed_volume(polyhedron)` の絶対値を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として、時間 $O(V + F)$、追加領域 $O(1)$。

## 注意点

- 凸性は要求しない。面が閉じていて、向きが一貫した三角形面であることを前提とし、入力の検査と例外は `polyhedron_signed_volume` に従う。
