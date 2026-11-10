---
title: Direct Minkowski Difference 3D (直接構築する三次元Minkowski差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_polyhedron/minkowski_difference_3d_direct.hpp
---

第 2 の多面体を原点について反転し、全頂点対を作らずに Minkowski 差を直接構築する。

## API

- `minkowski_difference_3d_direct(first, second, general_position_margin = 1e-9L)`: 反転した第 2 項と第 1 項の facet・幾何学的な辺・支持頂点から差を構築する（反転後に `minkowski_sum_3d_direct` を呼ぶ）。

## API 別の時間計算量・空間計算量

頂点数を $V_i$、三角形面数を $T_i$、統合した facet の数を $F_i$、幾何学的な辺の数を $E_i$、出力サイズを $K$ とする。期待時間は $O(T_1 + T_2 + F_1 V_2 + F_2 V_1 + E_1 E_2 + K)$、追加領域は $O(V_1 + V_2 + T_1 + T_2 + K)$。第 2 項の反転にかかる $O(V_2 + T_2)$ はこの上界に含まれる。

## 注意点

- 両入力は full-dimensional で、閉じた凸境界を外向きかつ整合した三角形で表していなければならない。
- 反転後の支持頂点・幾何学的な辺・法線錐が `general_position_margin` 以上離れている一般位置を要求する。
- `general_position_margin` は開区間 $(0, 0.01)$ に入る有限値でなければならない。範囲外や非有限な入力、次元が 3 でない入力には `std::invalid_argument`、一般位置などの契約違反には `std::domain_error`、座標の差のオーバーフローには `std::overflow_error` を送出する。
- 低速版へのフォールバックは行わない。
