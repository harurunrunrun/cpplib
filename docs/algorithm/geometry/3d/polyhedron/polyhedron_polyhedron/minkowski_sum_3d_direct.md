---
title: Direct Minkowski Sum 3D (直接構築する三次元Minkowski和)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_polyhedron/minkowski_sum_3d_direct.hpp
---

2 つの凸多面体の facet・幾何学的な辺・支持頂点を組み合わせ、全頂点対を作らずに Minkowski 和を構築する。

## API

- `minkowski_sum_3d_direct(first, second, general_position_margin = 1e-9L)`: facet に由来する面と、幾何学的な辺の法線錐の交差に由来する面を直接接続して返す。

## API 別の時間計算量・空間計算量

頂点数を $V_i$、三角形面数を $T_i$、統合した facet の数を $F_i$、幾何学的な辺の数を $E_i$、出力サイズを $K$ とする。期待時間は $O(T_1 + T_2 + F_1 V_2 + F_2 V_1 + E_1 E_2 + K)$、追加領域は $O(V_1 + V_2 + T_1 + T_2 + K)$。ハッシュ表の衝突が集中する場合は、この期待上界を保証しない。

## 注意点

- 両入力は full-dimensional で、閉じた凸境界を外向きかつ整合した三角形で表していなければならない。
- 相手の facet 法線に対して支持頂点が一意であり、異なる入力の幾何学的な辺と法線錐が `general_position_margin` 以上離れている一般位置を要求する。
- `general_position_margin` は開区間 $(0, 0.01)$ に入る有限値でなければならない。範囲外や非有限な入力、次元が 3 でない入力には `std::invalid_argument`、一般位置などの契約違反には `std::domain_error`、座標の和のオーバーフローには `std::overflow_error` を送出する。
- 低速版へのフォールバックは行わない。
