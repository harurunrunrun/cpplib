---
title: Minkowski Difference 3D (三次元Minkowski差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_polyhedron/minkowski_difference_3d_default.hpp
---

第 2 入力を原点について反転し、一般位置では法線扇（normal fan）を直接マージする。法線扇が退化している場合や低次元の入力では、全頂点対の差の凸包へ自動的に切り替える。

## API

- `minkowski_difference_3d(first, second)`: 集合 $\lbrace a - b \mid a \in A, b \in B \rbrace$ の凸包を、再現可能な頂点列・面列を持つ `ConvexPolyhedron3` として返す。

## API 別の時間計算量・空間計算量

$V_i, E_i, F_i$ を各入力の頂点・辺・面数、$N_i = V_i + E_i + F_i$、$K$ を出力サイズとする。

- 一般位置での直接構築: $O(F_1 V_2 + F_2 V_1 + E_1 E_2 + K) = O(N_1 N_2 + K)$ 時間、$O(N_1 + N_2 + K)$ 追加領域。
- 退化時のフォールバック: $P = V_1 V_2$ として、全頂点対の差を候補点とし、決定的な `convex_hull_3d` で最悪 $O(P \log P + K)$ 時間、$O(P + K)$ 追加領域。候補点が 256 個以下なら、まず `quickhull_3d` の結果を厳密に検証して使う。

## 注意点

- 両入力は空でなく、全頂点が有限でなければならない。空の入力や非有限な頂点には `std::invalid_argument` を、候補数が `std::vector` の上限を超える場合は `std::length_error` を送出する。
- 直接構築が一般位置の条件を満たさない場合（`std::domain_error`）は、例外を外へ出さずに凸包フォールバックで退化した次元と重複点を処理する。
- 座標の差が `long double` で表現できない場合は、例外（`std::overflow_error` または `std::invalid_argument`）が送出され得る。
