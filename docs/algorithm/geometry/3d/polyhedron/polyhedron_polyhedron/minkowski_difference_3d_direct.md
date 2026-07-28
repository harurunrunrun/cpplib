---
title: Direct Minkowski Difference 3D (直接構築する三次元Minkowski差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_polyhedron/minkowski_difference_3d_direct.hpp
---

第2多面体を原点反転し、全頂点対を作らずMinkowski差を直接構築する。

## API

- `minkowski_difference_3d_direct(first, second, general_position_margin)`:
  反転した第2項と第1項のfacet・幾何辺・支持頂点から差を構築する。

## API別の時間計算量・空間計算量

頂点数を $V_i$、三角形面数を $T_i$、統合facet数を $F_i$、幾何辺数を
$E_i$、出力サイズを $K$ とする。期待時間は
$O(T_1+T_2+F_1V_2+F_2V_1+E_1E_2+K)$、追加領域は
$O(V_1+V_2+T_1+T_2+K)$。第2項の反転 $O(V_2+T_2)$ はこの上界に含む。

## 注意点

両入力はfull-dimensionalで、閉じた凸境界を外向きかつ整合した三角形で表す
必要がある。反転後の支持頂点・幾何辺・法線錐が
`general_position_margin` 以上離れる一般位置を要求する。
marginは有限な $(0,0.01)$。契約違反は `std::domain_error`、
非有限入力・次元違いは `std::invalid_argument`、座標差のoverflowは
`std::overflow_error`。低速版へfallbackしない。
