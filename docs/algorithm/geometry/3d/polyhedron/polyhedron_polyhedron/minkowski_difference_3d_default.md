---
title: Minkowski Difference 3D (三次元Minkowski差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_polyhedron/minkowski_difference_3d_default.hpp
---

第二入力を原点について反転し、一般位置では法線扇を直接マージする。
退化した法線扇または低次元入力では、全頂点対の差の凸包へ自動的に切り替える。

## API

- `minkowski_difference_3d(first, second)`: $\{a-b\mid a\in A,b\in B\}$ の
  凸包を再現可能な頂点列・面列を持つ `ConvexPolyhedron3` として返す。

## API別の時間計算量・空間計算量

$V_i,E_i,F_i$ を各入力の頂点・辺・面数、$N_i=V_i+E_i+F_i$、
$K$ を出力サイズとする。一般位置の直接構築は
$O(F_1V_2+F_2V_1+E_1E_2+K)=O(N_1N_2+K)$ 時間、
$O(N_1+N_2+K)$ 追加領域。

退化時のfallbackは $P=V_1V_2$ として、固定seedに対する最悪
$O(P^2\log P+K)$ 時間・$O(P^2+K)$ 追加領域。固定seedに敵対しない入力では
期待 $O(P\log P+K)$ 時間・$O(P+K)$ 追加領域。

## 注意点

両入力は空でなく、全頂点が有限でなければならない。空入力は
`std::invalid_argument`、候補数がvector上限を超えると
`std::length_error`。直接構築が一般位置条件を満たさない場合は例外を外へ出さず、
凸包fallbackで退化次元と重複点を処理する。
