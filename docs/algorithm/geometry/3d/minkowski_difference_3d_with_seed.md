---
title: Minkowski Difference 3D with Seed (seed指定の三次元Minkowski差)
documentation_of: ../../../../src/algorithm/geometry/3d/minkowski_difference_3d_with_seed.hpp
---

全頂点対の差を候補とし、指定seedの三次元凸包としてMinkowski差を構築する。

## API

- `minkowski_difference_3d_with_seed(first, second, random_seed)`:
  候補点 $a-b$ の凸包を返す。増分順序だけを `random_seed` で指定する。

## API別の時間計算量・空間計算量

$V_1,V_2$ を頂点数、$P=V_1V_2$、$K$ を出力サイズとする。seedを入力と
独立に選ぶと期待時間 $O(P\log P+K)$、期待追加領域 $O(P+K)$。
固定seedに対する最悪時間は $O(P^2\log P+K)$、最悪追加領域
$O(P^2+K)$。候補生成だけなら $O(P)$ 時間・領域。

## 注意点

seedは結果形状を変えず、頂点・面の列挙順に影響し得る。両入力は空でなく
全座標が有限でなければならない。空入力は `std::invalid_argument`、
候補数超過は `std::length_error`。
