---
title: Direct Minkowski Sum 3D (直接構築する三次元Minkowski和)
documentation_of: ../../../../src/algorithm/geometry/3d/minkowski_sum_3d_direct.hpp
---

二つの凸多面体のfacet・幾何辺・支持頂点を組み合わせ、全頂点対を作らず和を構築する。

## API

- `minkowski_sum_3d_direct(first, second, general_position_margin)`:
  facet由来の面、幾何辺の法線錐交差由来の面を直接接続して返す。

## API別の時間計算量・空間計算量

頂点数を $V_i$、三角形面数を $T_i$、統合facet数を $F_i$、幾何辺数を
$E_i$、出力サイズを $K$ とする。期待時間は
$O(T_1+T_2+F_1V_2+F_2V_1+E_1E_2+K)$、追加領域は
$O(V_1+V_2+T_1+T_2+K)$。hash tableの衝突が集中する場合はこの期待上界を
保証しない。

## 注意点

両入力はfull-dimensionalで、閉じた凸境界を外向きかつ整合した三角形で表す
必要がある。相手のfacet法線が一意な支持頂点を持ち、異なる入力の幾何辺と
法線錐が `general_position_margin` 以上離れる一般位置を要求する。
marginは有限な $(0,0.01)$。契約違反は `std::domain_error`、
非有限入力・次元違いは `std::invalid_argument`、座標和のoverflowは
`std::overflow_error`。低速版へfallbackしない。
