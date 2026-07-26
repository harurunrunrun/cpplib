---
title: Minkowski Difference 3D with Default Seed (固定seedによる三次元Minkowski差)
documentation_of: ../../../../src/algorithm/geometry/3d/minkowski_difference_3d_default.hpp
---

全頂点対の差を候補とし、固定seedの三次元凸包としてMinkowski差を構築する。

## API

- `minkowski_difference_3d(first, second)`: $\{a-b\mid a\in A,b\in B\}$ の
  凸包を再現可能な頂点列・面列を持つ `ConvexPolyhedron3` として返す。

## API別の時間計算量・空間計算量

$V_1,V_2$ を頂点数、$P=V_1V_2$、$K$ を出力サイズとする。候補生成は
$O(P)$ 時間・領域。固定seedに対する全体の最悪時間は
$O(P^2\log P+K)$、最悪追加領域は $O(P^2+K)$。既定seedに敵対しない
入力では期待 $O(P\log P+K)$ 時間・$O(P+K)$ 領域。

## 注意点

両入力は空でなく、全頂点が有限でなければならない。空入力は
`std::invalid_argument`、候補数がvector上限を超えると
`std::length_error`。退化次元と重複点は凸包側で処理する。
