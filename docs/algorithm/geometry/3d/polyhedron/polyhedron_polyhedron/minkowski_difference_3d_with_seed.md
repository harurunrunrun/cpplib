---
title: Minkowski Difference 3D with Seed (seed指定の三次元Minkowski差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_polyhedron/minkowski_difference_3d_with_seed.hpp
---

Minkowski 差を構築する。両入力が三次元なら `minkowski_difference_3d` と同じ経路（直接構築と決定的なフォールバック）を使い、そうでない場合は全頂点対の差を候補点として、指定したシードの乱択増分法による三次元凸包で構築する。

## API

- `minkowski_difference_3d_with_seed(first, second, random_seed)`: 候補点 $a - b$（$a \in A, b \in B$）の凸包を返す。`random_seed` は低次元の入力を含む場合の凸包構築の増分順序だけを決める。

## API 別の時間計算量・空間計算量

$V_1, V_2$ を頂点数、$P = V_1 V_2$、$K$ を出力サイズとする。

- 両入力が三次元: `minkowski_difference_3d` と同じ。
- それ以外: シードを入力と独立に選ぶと期待時間 $O(P \log P + K)$、期待追加領域 $O(P + K)$。固定シードに対する最悪時間は $O(P^2 \log P + K)$、最悪追加領域は $O(P^2 + K)$。候補生成だけなら $O(P)$ 時間・領域。

## 注意点

- シードは結果の形状を変えず、頂点・面の列挙順にだけ影響し得る。両入力が三次元の場合はシードを使わない。
- 両入力は空でなく、全座標が有限でなければならない。空の入力には `std::invalid_argument`、候補数が `std::vector` の上限を超える場合は `std::length_error` を送出する。
